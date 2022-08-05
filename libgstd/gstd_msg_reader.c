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

#include "gstd_msg_reader.h"
#include "gstd_property_reader.h"
#include "gstd_pipeline_bus.h"
#include "gstd_bus_msg.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_msg_reader_debug);
#define GST_CAT_DEFAULT gstd_msg_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_msg_reader_read (GstdIReader * iface,
    GstdObject * object, const gchar * name, GstdObject ** out);

static GstdReturnCode
gstd_msg_reader_read_message (GstdIReader * iface,
    GstdObject * object, GstdObject ** out);

typedef struct _GstdMsgReaderClass GstdMsgReaderClass;

struct _GstdMsgReader
{
  GstdPropertyReader parent;
};

struct _GstdMsgReaderClass
{
  GstdPropertyReaderClass parent_class;
};


static GstdIReaderInterface *parent_interface = NULL;

static void
gstd_ireader_interface_init (GstdIReaderInterface * iface)
{
  parent_interface = g_type_interface_peek_parent (iface);

  iface->read = gstd_msg_reader_read;
}

G_DEFINE_TYPE_WITH_CODE (GstdMsgReader, gstd_msg_reader,
    GSTD_TYPE_PROPERTY_READER, G_IMPLEMENT_INTERFACE (GSTD_TYPE_IREADER,
        gstd_ireader_interface_init));

static void
gstd_msg_reader_class_init (GstdMsgReaderClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_msg_reader_debug, "gstdmsgreader",
      debug_color, "Gstd Message Reader category");
}

static void
gstd_msg_reader_init (GstdMsgReader * self)
{
  GST_INFO_OBJECT (self, "Initializing message reader");
}

static GstdReturnCode
gstd_msg_reader_read (GstdIReader * iface, GstdObject * object,
    const gchar * name, GstdObject ** out)
{
  GstdReturnCode ret;
  GstdObject *resource = NULL;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  /* If the user requested to read the message, read from the bus,
   * else, default to the property reading implementation
   */
  if (!g_ascii_strcasecmp ("message", name)) {
    ret = gstd_msg_reader_read_message (iface, object, &resource);
  } else {
    ret = parent_interface->read (iface, object, name, &resource);
  }

  if (!ret) {
    *out = resource;
  }

  return ret;
}

static GstdReturnCode
gstd_msg_reader_read_message (GstdIReader * iface,
    GstdObject * object, GstdObject ** out)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdPipelineBus *gstdbus;
  GstBus *bus;
  gint64 timeout;
  gint types;
  GstMessage *msg;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_PIPELINE_BUS (object), GSTD_BAD_VALUE);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  gstdbus = GSTD_PIPELINE_BUS (object);

  bus = gstd_pipeline_bus_get_bus (gstdbus);
  g_object_get (gstdbus, "timeout", &timeout, NULL);
  g_object_get (gstdbus, "types", &types, NULL);

  /* The unknown or none message type is not a valid polling filter,
   * instead we interpret it as a flushing request. As such we flush
   * the bus for "timeout" nanoseconds
   */
  if (GST_MESSAGE_UNKNOWN == types) {
    GST_INFO_OBJECT (gstdbus, "Flushing the bus for %" GST_TIME_FORMAT,
        GST_TIME_ARGS (timeout));
    gst_bus_set_flushing (bus, TRUE);
    g_usleep (GST_TIME_AS_USECONDS (timeout));
    gst_bus_set_flushing (bus, FALSE);
    msg = NULL;
  } else {
    msg = gst_bus_timed_pop_filtered (bus, timeout, types);
  }

  if (msg) {
    *out = GSTD_OBJECT (gstd_bus_msg_factory_make (msg));
  }

  gst_object_unref (bus);

  return ret;
}
