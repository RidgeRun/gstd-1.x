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

#include "gstd_bus_msg.h"
#include "gstd_bus_msg_qos.h"

/* Gstd Bus MsgQos debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_qos_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_qos_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_bus_msg_qos_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target);

struct _GstdBusMsgQos
{
  GstdBusMsg parent;
};

struct _GstdBusMsgQosClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgQos, gstd_bus_msg_qos, GSTD_TYPE_BUS_MSG);

static void
gstd_bus_msg_qos_class_init (GstdBusMsgQosClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_qos_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_qos_debug, "gstdbusmsgqos", debug_color,
      "Gstd Bus Msg Qos category");

}

static void
gstd_bus_msg_qos_init (GstdBusMsgQos * self)
{
  GST_INFO_OBJECT (self, "Initializing bus QOS message");
}

static GstdReturnCode
gstd_bus_msg_qos_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  gboolean live;
  guint64 running_time;
  guint64 stream_time;
  guint64 timestamp;
  guint64 duration;
  gint64 jitter;
  gdouble proportion;
  gint quality;
  GstFormat format;
  guint64 processed;
  guint64 dropped;
  GValue value = G_VALUE_INIT;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  gst_message_parse_qos (target, &live, &running_time, &stream_time, &timestamp,
      &duration);
  gst_message_parse_qos_values (target, &jitter, &proportion, &quality);
  gst_message_parse_qos_stats (target, &format, &processed, &dropped);

  gstd_iformatter_set_member_name (formatter, "buffer");
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "live");
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, live);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "running_time");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, running_time);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "stream_time");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, stream_time);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "timestamp");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, timestamp);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "duration");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, duration);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_end_object (formatter);

  gstd_iformatter_set_member_name (formatter, "values");
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "jitter");
  g_value_init (&value, G_TYPE_INT64);
  g_value_set_int64 (&value, jitter);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "proportion");
  g_value_init (&value, G_TYPE_DOUBLE);
  g_value_set_double (&value, proportion);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "quality");
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, quality);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_end_object (formatter);

  gstd_iformatter_set_member_name (formatter, "stats");
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "format");
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, format);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "processed");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, processed);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "dropped");
  g_value_init (&value, G_TYPE_UINT64);
  g_value_set_uint64 (&value, dropped);
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  gstd_iformatter_end_object (formatter);

  return GSTD_EOK;
}
