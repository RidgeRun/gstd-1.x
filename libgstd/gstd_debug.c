/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gprintf.h>

#include "gstd_debug.h"
#include "gstd_object.h"
#include "gstd_property_reader.h"

GST_DEBUG_CATEGORY_STATIC (gstd_debug_cat);

enum
{
  PROP_ENABLE = 1,
  PROP_COLOR,
  PROP_THRESHOLD,
  PROP_FLAGS,
  PROP_RESET,
  N_PROPERTIES                  // NOT A PROPERTY
};

#define PROP_RESET_DEFAULT    TRUE

struct _GstdDebug
{
  GstdObject parent;

  /*
   * Enables/Disables debug output.
   */
  gboolean enable;

  /*
   * Enables/Disables debug output color.
   */
  gboolean color;

  /*
   * Current threshold level
   */
  gchar *threshold;

  /*
   * Enables/Disables debug reset
   */
  gboolean reset;

  GParamFlags flags;
};

struct _GstdDebugClass
{
  GstdObjectClass parent_class;
};

G_DEFINE_TYPE (GstdDebug, gstd_debug, GSTD_TYPE_OBJECT);

/* VTable */
static void
gstd_debug_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_debug_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_debug_dispose (GObject *);

static gchar *
debug_obtain_default_level (void)
{
  gint level = gst_debug_get_default_threshold ();
  return g_strdup_printf ("%d", level);
}

static void
gstd_debug_class_init (GstdDebugClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  gchar *temp_threshold = debug_obtain_default_level ();
  guint debug_color;

  object_class->set_property = gstd_debug_set_property;
  object_class->get_property = gstd_debug_get_property;
  object_class->dispose = gstd_debug_dispose;

  properties[PROP_ENABLE] =
      g_param_spec_boolean ("enable",
      "Enable",
      "Current gstreamer debug enabled state",
      gst_debug_is_active (), G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_COLOR] =
      g_param_spec_boolean ("color",
      "Color",
      "Current gstreamer debug color enabled state",
      gst_debug_is_colored (), G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_THRESHOLD] =
      g_param_spec_string ("threshold",
      "Threshold",
      "The gstreamer debug level threshold",
      temp_threshold, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_FLAGS] =
      g_param_spec_flags ("flags",
      "Flags",
      "The resource access flags",
      GSTD_TYPE_PARAM_FLAGS,
      GSTD_PARAM_READ | GSTD_PARAM_UPDATE,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_RESET] =
      g_param_spec_boolean ("reset",
      "Reset",
      "Clear previously set debug thresholds ",
      PROP_RESET_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug_cat, "gstddebug", debug_color,
      "Gstd debug category");

  g_free (temp_threshold);
}

static void
gstd_debug_init (GstdDebug * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd debug");

  self->enable = gst_debug_is_active ();
  self->color = gst_debug_is_colored ();
  self->threshold = debug_obtain_default_level ();
  self->reset = PROP_RESET_DEFAULT;

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));
}

static void
gstd_debug_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdDebug *self = GSTD_DEBUG (object);

  switch (property_id) {

    case PROP_ENABLE:
      self->enable = gst_debug_is_active ();
      GST_DEBUG_OBJECT (self, "Returning debug enabled %d", self->enable);
      g_value_set_boolean (value, self->enable);
      break;
    case PROP_COLOR:
      self->color = gst_debug_is_colored ();
      GST_DEBUG_OBJECT (self, "Returning debug colored %d", self->color);
      g_value_set_boolean (value, self->color);
      break;
    case PROP_THRESHOLD:
      GST_DEBUG_OBJECT (self, "Returning debug level threshold %s",
          self->threshold);
      g_value_set_string (value, self->threshold);
      break;
    case PROP_FLAGS:
      GST_DEBUG_OBJECT (self, "Returning flags %u", self->flags);
      g_value_set_flags (value, self->flags);
      break;
    case PROP_RESET:
      GST_WARNING_OBJECT (self, "Returning debug reset %d", self->reset);
      g_value_set_boolean (value, self->reset);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_debug_set_threshold (GstdDebug * self)
{
  /* If reset is set, clear all the categories to 0. We run this manually
   * since the GStreamer gst_debug_set_threshold_from_string reset only clears
   * the categories that weren't set explicitly. Otherwise the threshold will
   * be appended to previously set thresholds
   */
  if (self->reset)
    gst_debug_set_threshold_from_string ("*:0", TRUE);

  gst_debug_set_threshold_from_string (self->threshold, FALSE);
}

static void
gstd_debug_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdDebug *self = GSTD_DEBUG (object);

  switch (property_id) {
    case PROP_ENABLE:
      self->enable = g_value_get_boolean (value);
      GST_DEBUG_OBJECT (self, "Changing debug enabled to %d", self->enable);
      gst_debug_set_active (self->enable);
      /* if debug was enabled and threshold value was previously set, then
       * set threshold value
       */
      if ((self->enable) && (self->threshold)) {
        gstd_debug_set_threshold (self);
      }
      break;
    case PROP_COLOR:
      self->color = g_value_get_boolean (value);
      GST_DEBUG_OBJECT (self, "Changing debug colored to %d", self->color);
      gst_debug_set_colored (self->color);
      break;
    case PROP_THRESHOLD:
      if (self->threshold) {
        g_free (self->threshold);
      }
      self->threshold = g_value_dup_string (value);
      GST_DEBUG_OBJECT (self, "Changing debug threshold to %s",
          self->threshold);
      /* Since debug is actived when the threshold is set, then set the threshold
       * value if debug is active
       */
      if (TRUE == gst_debug_is_active ()) {
        gstd_debug_set_threshold (self);
      }
      break;
    case PROP_RESET:
      self->reset = g_value_get_boolean (value);
      GST_DEBUG_OBJECT (self, "Changing debug reset to %d", self->reset);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_debug_dispose (GObject * object)
{
  GstdDebug *self = GSTD_DEBUG (object);
  GST_INFO_OBJECT (object, "Deinitializing gstd debug");
  G_OBJECT_CLASS (gstd_debug_parent_class)->dispose (object);

  if (self->threshold) {
    g_free (self->threshold);
    self->threshold = NULL;
  }
}


GstdDebug *
gstd_debug_new (void)
{
  return GSTD_DEBUG (g_object_new (GSTD_TYPE_DEBUG, NULL));
}
