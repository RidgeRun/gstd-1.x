/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
