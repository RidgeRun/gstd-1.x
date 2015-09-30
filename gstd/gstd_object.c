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
#include "gstd_object.h"

enum {
  PROP_NAME = 1,
  N_PROPERTIES // NOT A PROPERTY
};

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_object_debug);
#define GST_CAT_DEFAULT gstd_object_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdObject, gstd_object, G_TYPE_OBJECT)

/* VTable */
static void
gstd_object_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_object_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_object_dispose (GObject *);

static void
gstd_object_class_init (GstdObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_object_set_property;
  object_class->get_property = gstd_object_get_property;
  object_class->dispose = gstd_object_dispose;

  properties[PROP_NAME] =
    g_param_spec_string ("name",
			 "Name",
			 "The name of the current Gstd session",
			 GSTD_OBJECT_DEFAULT_NAME,
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_READWRITE |
			 G_PARAM_STATIC_STRINGS);
  
  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);
  
  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_object_debug, "gstdobject", debug_color,
			   "Gstd Object category");
}

static void
gstd_object_init (GstdObject *self)
{
  GST_DEBUG_OBJECT(self, "Initializing gstd object");

  self->name = g_strdup(GSTD_OBJECT_DEFAULT_NAME);
}

static void
gstd_object_get_property (GObject        *object,
		   guint           property_id,
		   GValue         *value,
		   GParamSpec     *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);

  switch (property_id) {
  case PROP_NAME:
    GST_DEBUG_OBJECT(self, "Returning object name \"%s\"", self->name);
    g_value_set_string (value, self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gstd_object_set_property (GObject      *object,
		   guint         property_id,
		   const GValue *value,
		   GParamSpec   *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  switch (property_id) {
  case PROP_NAME:
    if (self->name)
      g_free(self->name);
    
    self->name = g_value_dup_string (value);
    GST_INFO_OBJECT(self, "Changed object name to %s", self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gstd_object_dispose (GObject *object)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  GST_DEBUG_OBJECT(object, "Deinitializing gstd object");

  if (self->name) {
    g_free (self->name);
    self->name = NULL;
  }
  
  G_OBJECT_CLASS(gstd_object_parent_class)->dispose(object);
}
