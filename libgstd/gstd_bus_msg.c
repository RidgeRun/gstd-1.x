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
#include "gstd_bus_msg_element.h"
#include "gstd_bus_msg_notify.h"
#include "gstd_bus_msg_qos.h"
#include "gstd_bus_msg_simple.h"
#include "gstd_bus_msg_state_changed.h"
#include "gstd_bus_msg_stream_status.h"

/* Gstd Bus Msg debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void gstd_bus_msg_dispose (GObject * object);
static GstdReturnCode gstd_bus_msg_to_string (GstdObject * object,
    gchar ** outstring);

G_DEFINE_TYPE (GstdBusMsg, gstd_bus_msg, GSTD_TYPE_OBJECT);

static void
gstd_bus_msg_class_init (GstdBusMsgClass * klass)
{
  GObjectClass *oclass;
  GstdObjectClass *goclass;

  guint debug_color;

  oclass = G_OBJECT_CLASS (klass);
  goclass = GSTD_OBJECT_CLASS (klass);

  oclass->dispose = gstd_bus_msg_dispose;
  goclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_to_string);
  klass->to_string = NULL;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_debug, "gstdbusmsg", debug_color,
      "Gstd Bus Msg category");

}

static void
gstd_bus_msg_init (GstdBusMsg * self)
{
  GST_INFO_OBJECT (self, "Initializing bus message");
}

static void
gstd_bus_msg_dispose (GObject * object)
{
  GstdBusMsg *self;

  self = GSTD_BUS_MSG (object);

  if (self->target) {
    gst_message_unref (self->target);
    self->target = NULL;
  }

  G_OBJECT_CLASS (gstd_bus_msg_parent_class)->dispose (object);
}

GstdBusMsg *
gstd_bus_msg_factory_make (GstMessage * target)
{
  GstdBusMsg *msg = NULL;
  GstMessageType type;

  g_return_val_if_fail (target, NULL);

  type = GST_MESSAGE_TYPE (target);

  switch (type) {
    case (GST_MESSAGE_ERROR):
    case (GST_MESSAGE_WARNING):
    case (GST_MESSAGE_INFO):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_SIMPLE, NULL);
      break;
    case (GST_MESSAGE_QOS):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_QOS, NULL);
      break;
    case (GST_MESSAGE_STREAM_STATUS):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_STREAM_STATUS, NULL);
      break;
    case (GST_MESSAGE_ELEMENT):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_ELEMENT, NULL);
      break;
    case (GST_MESSAGE_STATE_CHANGED):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_STATE_CHANGED, NULL);
      break;
#if GST_VERSION_MINOR >= 10
    case (GST_MESSAGE_PROPERTY_NOTIFY):
      msg = g_object_new (GSTD_TYPE_BUS_MSG_NOTIFY, NULL);
      break;
#endif
    default:
      msg = g_object_new (GSTD_TYPE_BUS_MSG, NULL);
      break;
  }

  if (msg) {
    msg->target = target;
  }

  return msg;
}

static GstdReturnCode
gstd_bus_msg_to_string (GstdObject * object, gchar ** outstring)
{
  GstdBusMsg *self;
  GstMessage *target;
  gchar *ts;
  GValue value = G_VALUE_INIT;
  GstdIFormatter *formatter = g_object_new (object->formatter_factory, NULL);

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_BUS_MSG (object);

  g_return_val_if_fail (self->target, GSTD_MISSING_INITIALIZATION);
  target = self->target;

  gstd_iformatter_begin_object (formatter);
  gstd_iformatter_set_member_name (formatter, "type");
  gstd_iformatter_set_string_value (formatter, GST_MESSAGE_TYPE_NAME (target));

  gstd_iformatter_set_member_name (formatter, "source");
  gstd_iformatter_set_string_value (formatter, GST_MESSAGE_SRC_NAME (target));

  ts = g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (target->timestamp));
  gstd_iformatter_set_member_name (formatter, "timestamp");
  gstd_iformatter_set_string_value (formatter, ts);
  g_free (ts);

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, target->seqnum);
  gstd_iformatter_set_member_name (formatter, "seqnum");
  gstd_iformatter_set_value (formatter, &value);
  g_value_unset (&value);

  if (GSTD_BUS_MSG_GET_CLASS (self)->to_string) {
    GSTD_BUS_MSG_GET_CLASS (self)->to_string (self, formatter, target);
  }

  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  /* Free formatter */
  g_object_unref (formatter);
  return GSTD_EOK;
}
