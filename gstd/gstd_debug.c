/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <glib/gprintf.h>

#include "gstd/gstd_debug.h"
#include "gstd/gstd_object.h"

GST_DEBUG_CATEGORY_STATIC(gstd_debug_cat);

enum {
  PROP_ENABLE = 1,
  PROP_COLOR,
  PROP_THRESHOLD,
  PROP_FLAGS,
  N_PROPERTIES // NOT A PROPERTY
};

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
  gchar* threshold;
  GParamFlags flags;
};

struct _GstdDebugClass
{
  GstdObjectClass parent_class;
};

G_DEFINE_TYPE (GstdDebug, gstd_debug, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_debug_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_debug_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_debug_dispose (GObject *);

char* debug_obtain_default_level()
{
  gint level = gst_debug_get_default_threshold();
  gsize length = (level == 0) ? 1 : (gsize) floor(log10(fabs((double)level))) + 1;
  gchar* buf = g_malloc (length*sizeof(gchar));
  g_sprintf(buf, "%d", level);
  return buf;
}

static void
gstd_debug_class_init (GstdDebugClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };

  object_class->set_property = gstd_debug_set_property;
  object_class->get_property = gstd_debug_get_property;
  object_class->dispose = gstd_debug_dispose;

  properties[PROP_ENABLE] =
    g_param_spec_boolean ("enable",
           "Enable",
           "Current gstreamer debug enabled state",
           gst_debug_is_active(),
           G_PARAM_READWRITE |
           G_PARAM_STATIC_STRINGS);

  properties[PROP_COLOR] =
    g_param_spec_boolean ("color",
           "Color",
           "Current gstreamer debug color enabled state",
           gst_debug_is_colored(),
           G_PARAM_READWRITE |
           G_PARAM_STATIC_STRINGS);

  properties[PROP_THRESHOLD] =
    g_param_spec_string ("threshold",
           "Threshold",
           "The gstreamer debug level threshold",
           debug_obtain_default_level(),
           G_PARAM_READWRITE |
           G_PARAM_STATIC_STRINGS);

  properties[PROP_FLAGS] =
    g_param_spec_flags ("flags",
          "Flags",
          "The resource access flags",
          GSTD_TYPE_PARAM_FLAGS,
          GSTD_PARAM_READ | GSTD_PARAM_UPDATE,
          G_PARAM_READABLE |
          G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);
  
  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug_cat, "gstd_debug_cat", debug_color,
			   "Gstd debug category");
}

static void
gstd_debug_init (GstdDebug *self)
{
  GST_INFO_OBJECT(self, "Initializing gstd debug");

  self->enable = gst_debug_is_active();
  self->color = gst_debug_is_colored();
  self->threshold = debug_obtain_default_level();
}

static void
gstd_debug_get_property (GObject        *object,
      guint           property_id,
      GValue         *value,
      GParamSpec     *pspec)
{
  GstdDebug *self = GSTD_DEBUG(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
    
  case PROP_ENABLE:
    self->enable = gst_debug_is_active();
    GST_DEBUG_OBJECT(self, "Returning debug enabled %d", self->enable);
    g_value_set_boolean (value, self->enable);
    break;
  case PROP_COLOR:
    self->color = gst_debug_is_colored();
    GST_DEBUG_OBJECT(self, "Returning debug colored %d", self->color);
    g_value_set_boolean (value, self->color);
    break;
  case PROP_THRESHOLD:
    GST_DEBUG_OBJECT(self, "Returning debug level threshold %s", self->threshold);
    g_value_set_string (value, self->threshold);
    break;

  case PROP_FLAGS:
    GST_DEBUG_OBJECT(self, "Returning flags %u", self->flags);
    g_value_set_flags (value, self->flags);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_debug_set_property (GObject      *object,
		   guint         property_id,
		   const GValue *value,
		   GParamSpec   *pspec)
{
  GstdDebug *self = GSTD_DEBUG(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {

  case PROP_ENABLE:
    self->enable = g_value_get_boolean(value);
    GST_DEBUG_OBJECT(self, "Changing debug enabled to %d", self->enable);
    gst_debug_set_active(self->enable);
    break;
  case PROP_COLOR:
    self->color = g_value_get_boolean(value);
    GST_DEBUG_OBJECT(self, "Changing debug colored to %d", self->color);
    gst_debug_set_colored (self->color);
    break;
  case PROP_THRESHOLD:
    if (self->threshold)
    {
      g_free(self->threshold);
    }
    self->threshold = g_value_dup_string(value);
    GST_DEBUG_OBJECT(self, "Changing debug threshold to %s", self->threshold);
    gst_debug_set_threshold_from_string(self->threshold, TRUE);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_debug_dispose (GObject *object)
{
  GstdDebug *self = GSTD_DEBUG(object);
  GST_INFO_OBJECT(object, "Deinitializing gstd debug");
  G_OBJECT_CLASS(gstd_debug_parent_class)->dispose(object);

  if (self->threshold)
  {
    g_free(self->threshold);
    self->threshold = NULL;
  }
}


GstdDebug *
gstd_debug_new (GstDebugLevel lvl)
{
  return GSTD_DEBUG(g_object_new (GSTD_TYPE_DEBUG, "level", GST_LEVEL_NONE, NULL));
}