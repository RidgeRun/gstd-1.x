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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_bus_msg.h"
#include "gstd_bus_msg_stream_status.h"

/* Gstd Bus MsgStreamStatus debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_stream_status_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_stream_status_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_bus_msg_stream_status_to_string (GstdBusMsg * msg,
    GstdIFormatter * formatter, GstMessage * target);

static const gchar
    * gstd_bus_msg_stream_status_code_to_string (GstStreamStatusType type);

struct _GstdBusMsgStreamStatus
{
  GstdBusMsg parent;
};

struct _GstdBusMsgStreamStatusClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgStreamStatus, gstd_bus_msg_stream_status,
    GSTD_TYPE_BUS_MSG);

static void
gstd_bus_msg_stream_status_class_init (GstdBusMsgStreamStatusClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_stream_status_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_stream_status_debug,
      "gstdbusmsgstream_status", debug_color,
      "Gstd Bus Msg StreamStatus category");

}

static void
gstd_bus_msg_stream_status_init (GstdBusMsgStreamStatus * self)
{
  GST_INFO_OBJECT (self, "Initializing bus Stream Status message");
}

static const gchar *
gstd_bus_msg_stream_status_code_to_string (GstStreamStatusType type)
{
  const gchar *type_name;

  switch (type) {
    case GST_STREAM_STATUS_TYPE_CREATE:
      type_name = "GST_STREAM_STATUS_TYPE_CREATE";
      break;
    case GST_STREAM_STATUS_TYPE_ENTER:
      type_name = "GST_STREAM_STATUS_TYPE_ENTER";
      break;
    case GST_STREAM_STATUS_TYPE_LEAVE:
      type_name = "GST_STREAM_STATUS_TYPE_LEAVE";
      break;
    case GST_STREAM_STATUS_TYPE_DESTROY:
      type_name = "GST_STREAM_STATUS_TYPE_DESTROY";
      break;
    case GST_STREAM_STATUS_TYPE_START:
      type_name = "GST_STREAM_STATUS_TYPE_START";
      break;
    case GST_STREAM_STATUS_TYPE_PAUSE:
      type_name = "GST_STREAM_STATUS_TYPE_PAUSE";
      break;
    case GST_STREAM_STATUS_TYPE_STOP:
      type_name = "GST_STREAM_STATUS_TYPE_STOP";
      break;
    default:
      type_name = "UNKNOWN";
      break;
  }

  return type_name;
}

static GstdReturnCode
gstd_bus_msg_stream_status_to_string (GstdBusMsg * msg,
    GstdIFormatter * formatter, GstMessage * target)
{
  GstStreamStatusType type;
  GValue value = G_VALUE_INIT;
  GstElement *owner;
  GstElementFactory *factory;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  gst_message_parse_stream_status (target, &type, &owner);

  gstd_iformatter_set_member_name (formatter, "stream_status");
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "type");
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, type);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "type_name");
  gstd_iformatter_set_string_value (formatter,
      gstd_bus_msg_stream_status_code_to_string (type));

  gstd_iformatter_set_member_name (formatter, "owner");
  gstd_iformatter_set_string_value (formatter, GST_OBJECT_NAME (owner));

  gstd_iformatter_set_member_name (formatter, "owner_factory");
  factory = gst_element_get_factory (owner);
  gstd_iformatter_set_string_value (formatter, GST_OBJECT_NAME (factory));

  gstd_iformatter_end_object (formatter);

  return GSTD_EOK;
}
