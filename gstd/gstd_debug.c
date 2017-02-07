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

#include "gstd/gstd_debug.h"
#include "gstd/gstd_object.h"

enum {
  PROP_LEVEL = 1,
  PROP_COLOR,
  PROP_LIST,
  PROP_FLAGS,
  N_PROPERTIES // NOT A PROPERTY
};

struct _GstdDebug
{
  GstdObject parent;
  
  GstDebugLevel level;
  GstDebugColorFlags color;
  gchar* list;
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
static void
gstd_debug_constructed (GObject *);
static GstdReturnCode
gstd_debug_to_string (GstdObject *, gchar **);

static void
gstd_debug_class_init (GstdDebugClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstd_object_class = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };

  object_class->set_property = gstd_debug_set_property;
  object_class->get_property = gstd_debug_get_property;
  object_class->dispose = gstd_debug_dispose;
  object_class->constructed = gstd_debug_constructed;
  
  gstd_object_class->to_string = gstd_debug_to_string;

  properties[PROP_LEVEL] =
    g_param_spec_enum ("level",
           "Level",
           "The current gstreamer debug level",
           gst_debug_level_get_type(),
           GST_LEVEL_NONE,
           G_PARAM_READWRITE |
           G_PARAM_STATIC_STRINGS);

  properties[PROP_COLOR] =
    g_param_spec_enum ("color",
           "Color",
           "The debug font color",
           gst_debug_color_flags_get_type(),
           GST_DEBUG_FG_BLACK | GST_DEBUG_BG_WHITE,
           G_PARAM_READWRITE |
           G_PARAM_STATIC_STRINGS);

  properties[PROP_LIST] =
    g_param_spec_string ("list",
           "List",
           "The gstreamer debug level list",
           "*:0", //Set all elements to debug level 0
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
  /*debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd_debug", debug_color,
			   "Gstd debug category");*/
}

static void
gstd_debug_init (GstdDebug *self)
{
  GST_INFO_OBJECT(self, "Initializing gstd debug");

  self->level = GST_LEVEL_NONE;
  self->color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  self->list = "*.0";
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
  case PROP_LEVEL:
    self->level = gst_debug_get_default_threshold();
    GST_DEBUG_OBJECT(self, "Returning debug level %d", self->level);
    g_value_set_enum (value, self->level);
    break;
  case PROP_COLOR:
    GST_DEBUG_OBJECT(self, "Returning debug color %d", self->color);
    g_value_set_enum (value, self->color);
    break;
  case PROP_LIST:
    GST_DEBUG_OBJECT(self, "Returning debug list %s", self->list);
    g_value_set_string (value, self->list);
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
  case PROP_LEVEL:
    GST_DEBUG_OBJECT(self, "Changing debug level to %d", self->level);
    g_print("Setting self->level from %d to %d \n", self->level, g_value_get_enum(value));
    self->level = g_value_get_enum(value);
    gst_debug_set_default_threshold(self->level);
    break;
  case PROP_COLOR:
    GST_DEBUG_OBJECT(self, "Changing debug color to %d", self->color);
    g_print("Setting self->color from %d to %d \n", self->color, g_value_get_enum(value));
    self->color = g_value_get_enum(value);
    break;
  case PROP_LIST:
    GST_DEBUG_OBJECT(self, "Changing debug list to %s", self->list);
    g_print("Setting self->list from %s to %s \n", self->list, g_value_get_string(value));
    self->list = g_value_get_string(value);
    gst_debug_set_threshold_from_string(self->list, TRUE);
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
}

static void
gstd_debug_constructed (GObject *object)
{
  GstdDebug *self = GSTD_DEBUG(object);
}


GstdDebug *
gstd_debug_new (GstDebugLevel lvl)
{
  return GSTD_DEBUG(g_object_new (GSTD_TYPE_DEBUG, "level", GST_LEVEL_NONE, NULL));
}

static GstdReturnCode
gstd_debug_to_string (GstdObject *object, gchar **outstring)
{
  GstdDebug *self = GSTD_DEBUG(object);
  gchar *props;

  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  /* Lets leverage the parent's class implementation */
  GSTD_OBJECT_CLASS(gstd_debug_parent_class)->to_string(GSTD_OBJECT(object), &props);
  // A little hack to remove the last bracket
  props[strlen(props)-2] = '\0';
  
  //*outstring = g_strdup_printf("{%s,\n level : %d,\n color %d: ,\n list %s: \n}", props, self->level, self->color, self->list);
  *outstring = g_strdup_printf("%d", self->level);

  g_free (props);

  return GSTD_EOK;
}