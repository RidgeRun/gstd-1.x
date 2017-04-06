/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#include <gst/gst.h>

#include "gstd_property_reader.h"
#include "gstd_object.h"
#include "gstd_property_int.h"
#include "gstd_property_string.h"
#include "gstd_property_boolean.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_reader_debug);
#define GST_CAT_DEFAULT gstd_property_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdObject * gstd_property_reader_read (GstdIReader * iface,
    GstdObject * object, const gchar * name);

static GstdObject *
gstd_property_mask_type (GstdObject * object, const gchar * name);

typedef struct _GstdPropertyReaderClass GstdPropertyReaderClass;

/**
 * GstdPropertyReader:
 * A wrapper for the conventional property_reader
 */
struct _GstdPropertyReader
{
  GObject parent;
};

struct _GstdPropertyReaderClass
{
  GObjectClass parent_class;
};


static void
gstd_ireader_interface_init (GstdIReaderInterface * iface)
{
  iface->read = gstd_property_reader_read;
}

G_DEFINE_TYPE_WITH_CODE (GstdPropertyReader, gstd_property_reader,
    G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE (GSTD_TYPE_IREADER,
        gstd_ireader_interface_init));

static void
gstd_property_reader_class_init (GstdPropertyReaderClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_reader_debug, "gstdpropertyreader",
      debug_color, "Gstd Pipeline Reader category");
}

static void
gstd_property_reader_init (GstdPropertyReader * self)
{
  GST_INFO_OBJECT (self, "Initializing property reader");
}

static GstdObject *
gstd_property_reader_read (GstdIReader * iface, GstdObject * object, const gchar * name)
{
    GObjectClass * klass;
    GParamSpec * pspec;
    GstdObject * resource;

    g_return_val_if_fail (iface, NULL);
    g_return_val_if_fail (object, NULL);
    g_return_val_if_fail (name, NULL);

    klass = G_OBJECT_GET_CLASS(object);
    pspec = g_object_class_find_property (klass, name);

    /* No such property in the object */
    if (!pspec) {
      GST_ERROR_OBJECT (iface, "No %s resource in %s",
          name, GSTD_OBJECT_NAME (object));
      return NULL;
    }

    /* Property is not readable */
    if (!GSTD_PARAM_IS_READ (pspec->flags) || !(pspec->flags & G_PARAM_READABLE)) {
      GST_ERROR_OBJECT (iface, "The resource %s is not readable", name);
      return NULL;
    }

    g_object_get(object, name, &resource, NULL);

    /* We only support gstd objects */
    if (!GSTD_IS_OBJECT(resource)) {
      return gstd_property_mask_type (object, name);
    }

    return resource;
}

static GstdObject *
gstd_property_mask_type (GstdObject * object, const gchar * name)
{
    GObjectClass * klass;
    GType type;
    GParamSpec * pspec;
    
    g_return_val_if_fail (object, NULL);
    g_return_val_if_fail (name, NULL);
    
    klass = G_OBJECT_GET_CLASS(object);
    pspec = g_object_class_find_property (klass, name);

    switch (pspec->value_type) {
      case G_TYPE_BOOLEAN:
      {
	type = GSTD_TYPE_PROPERTY_BOOLEAN;
	break;
      }	
      case G_TYPE_INT:
      case G_TYPE_UINT:
      case G_TYPE_UINT64:
      case G_TYPE_INT64:
      {
	type = GSTD_TYPE_PROPERTY_INT;
	break;
      }	  
      case G_TYPE_STRING:
      {
	type = GSTD_TYPE_PROPERTY_STRING;
	break;	
      }
      default:
      {
	return NULL;
      }
    }

    return GSTD_OBJECT(g_object_new(type, "name", pspec->name, "target", object, NULL));
}
