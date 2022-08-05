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

#include "gstd_ireader.h"
#include "gstd_list_reader.h"
#include "gstd_object.h"
#include "gstd_list.h"
#include "gstd_property_int.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_list_reader_debug);
#define GST_CAT_DEFAULT gstd_list_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_list_reader_read (GstdIReader * iface, GstdObject * object,
    const gchar * name, GstdObject ** out);

static GstdReturnCode
gstd_list_reader_read_count (GstdIReader * iface,
    GstdObject * object, GstdObject ** out);

static GstdReturnCode
gstd_list_reader_read_child (GstdIReader * iface,
    GstdObject * object, const gchar * name, GstdObject ** out);

typedef struct _GstdListReaderClass GstdListReaderClass;

/**
 * GstdListReader:
 * A wrapper for the conventional list_reader
 */
struct _GstdListReader
{
  GObject parent;
};

struct _GstdListReaderClass
{
  GObjectClass parent_class;
};


static void
gstd_ireader_interface_init (GstdIReaderInterface * iface)
{
  iface->read = gstd_list_reader_read;
}

G_DEFINE_TYPE_WITH_CODE (GstdListReader, gstd_list_reader,
    G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE (GSTD_TYPE_IREADER,
        gstd_ireader_interface_init));

static void
gstd_list_reader_class_init (GstdListReaderClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_list_reader_debug, "gstdlistreader",
      debug_color, "Gstd Pipeline Reader category");
}

static void
gstd_list_reader_init (GstdListReader * self)
{
  GST_INFO_OBJECT (self, "Initializing list reader");
}

static GstdReturnCode
gstd_list_reader_read (GstdIReader * iface, GstdObject * object,
    const gchar * name, GstdObject ** out)
{
  GstdObject *resource;
  GstdReturnCode ret;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_LIST (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  /* In the case of lists, "count" is a keyword */
  if (!g_strcmp0 ("count", name)) {
    ret = gstd_list_reader_read_count (iface, object, &resource);
  } else {
    ret = gstd_list_reader_read_child (iface, object, name, &resource);
  }

  if (!resource) {
    GST_ERROR_OBJECT (iface, "No resource %s in %s", name,
        GST_OBJECT_NAME (object));
    goto out;
  }

  if (!GSTD_IS_OBJECT (resource)) {
    GST_ERROR_OBJECT (iface, "%s is not a valid resource", name);
    g_object_unref (resource);
    resource = NULL;
  }

out:
  *out = resource;
  return ret;
}


static GstdReturnCode
gstd_list_reader_read_count (GstdIReader * iface,
    GstdObject * object, GstdObject ** out)
{
  GstdPropertyInt *count_value;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  count_value = g_object_new (GSTD_TYPE_PROPERTY_INT,
      "name", "count", "target", object, NULL);

  *out = GSTD_OBJECT (count_value);

  return GSTD_EOK;
}

static GstdReturnCode
gstd_list_reader_read_child (GstdIReader * iface,
    GstdObject * object, const gchar * name, GstdObject ** out)
{
  gpointer found = NULL;
  GstdReturnCode ret;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  found = gstd_list_find_child (GSTD_LIST (object), name);
  if (found) {
    *out = GSTD_OBJECT (g_object_ref (found));
    ret = GSTD_EOK;
  } else {
    *out = NULL;
    ret = GSTD_NO_RESOURCE;
  }

  return ret;
}
