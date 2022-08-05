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

#include <gst/gst.h>

#include "gstd_property_reader.h"
#include "gstd_object.h"
#include "gstd_property_int.h"
#include "gstd_property_string.h"
#include "gstd_property_boolean.h"
#include "gstd_property_enum.h"
#include "gstd_property_flags.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_reader_debug);
#define GST_CAT_DEFAULT gstd_property_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode gstd_property_reader_read (GstdIReader * iface,
    GstdObject * object, const gchar * name, GstdObject ** out);

static GstdReturnCode
gstd_property_mask_type (GstdObject * object, const gchar * name,
    GstdObject ** out);
static gboolean gstd_property_reader_is_gstd (GParamSpec * pspec,
    GstdObject * object);

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

static GstdReturnCode
gstd_property_reader_read (GstdIReader * iface, GstdObject * object,
    const gchar * name, GstdObject ** out)
{
  GObjectClass *klass;
  GParamSpec *pspec;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  klass = G_OBJECT_GET_CLASS (object);
  pspec = g_object_class_find_property (klass, name);

  /* No such property in the object */
  if (!pspec) {
    GST_ERROR_OBJECT (iface, "No %s resource in %s",
        name, GSTD_OBJECT_NAME (object));
    return GSTD_NO_RESOURCE;
  }

  /* Property is not readable */
  if (!GSTD_PARAM_IS_READ (pspec->flags) || !(pspec->flags & G_PARAM_READABLE)) {
    GST_ERROR_OBJECT (iface, "The resource %s is not readable", name);
    return GSTD_NO_READ;
  }
  if (gstd_property_reader_is_gstd (pspec, object)) {
    g_object_get (object, name, out, NULL);
  } else {
    ret = gstd_property_mask_type (object, name, out);
  }
  return ret;
}

static GstdReturnCode
gstd_property_mask_type (GstdObject * object, const gchar * name,
    GstdObject ** out)
{
  GObjectClass *klass;
  GType type;
  GParamSpec *pspec;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  klass = G_OBJECT_GET_CLASS (object);
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
      if (G_TYPE_IS_ENUM (pspec->value_type)) {
        type = GSTD_TYPE_PROPERTY_ENUM;
      } else if (G_TYPE_IS_FLAGS (pspec->value_type)) {
        type = GSTD_TYPE_PROPERTY_FLAGS;
      } else {
        type = GSTD_TYPE_PROPERTY;
      }
    }
  }

  //FIXME:
  //I just found a way to handle all types in a generic way, hence,
  //the base property class can handle them all. I don't want to remove
  //specific type sublasses because the to_string method may require to
  //add details. For example, int properties can display their max and min
  //values, flags and enums could display the options, etc... Similar to
  //what gst-inspect does
  type = GSTD_TYPE_PROPERTY;

  *out =
      GSTD_OBJECT (g_object_new (type, "name", pspec->name, "target", object,
          NULL));

  return GSTD_EOK;
}

static gboolean
gstd_property_reader_is_gstd (GParamSpec * pspec, GstdObject * object)
{
  gboolean ret = FALSE;
  GObject *resource;

  if (G_TYPE_IS_DERIVED (pspec->value_type)
      && !G_TYPE_IS_ENUM (pspec->value_type)
      && !G_TYPE_IS_FLAGS (pspec->value_type)) {
    g_object_get (object, pspec->name, &resource, NULL);
    if (GSTD_IS_OBJECT (resource)) {
      ret = TRUE;
    }
    g_object_unref (resource);
  }

  return ret;
}
