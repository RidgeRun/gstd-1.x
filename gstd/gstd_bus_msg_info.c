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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_bus_msg.h"
#include "gstd_bus_msg_info.h"

/* Gstd Bus MsgInfo debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_bus_msg_info_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_info_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_bus_msg_info_to_string (GstdObject * self, gchar ** outstring);

struct _GstdBusMsgInfo
{
  GstdBusMsg parent;
};

struct _GstdBusMsgInfoClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgInfo, gstd_bus_msg_info, GSTD_TYPE_BUS_MSG)

static void
gstd_bus_msg_info_class_init (GstdBusMsgInfoClass *klass)
{
  GstdObjectClass * oclass;
  guint debug_color;

  oclass = GSTD_OBJECT_CLASS (klass);

  oclass->to_string = gstd_bus_msg_info_to_string;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_info_debug, "gstdbusmsginfo", debug_color,
			   "Gstd Bus Msg Info category");

}

static void
gstd_bus_msg_info_init (GstdBusMsgInfo *self)
{
  GST_INFO_OBJECT(self, "Initializing bus info/error/warning message");
}

static GstdReturnCode
gstd_bus_msg_info_to_string (GstdObject * object, gchar ** outstring)
{
  GstdBusMsg * msg;
  GstMessage * target;
  gchar * ts;
  GValue value = G_VALUE_INIT;
  GError * error;
  gchar * debug;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  msg = GSTD_BUS_MSG (object);

  g_return_val_if_fail (msg->target, GSTD_MISSING_INITIALIZATION);
  target = msg->target;

  gstd_iformatter_begin_object (object->formatter);
  gstd_iformatter_set_member_name (object->formatter,"type");
  gstd_iformatter_set_string_value (object->formatter, GST_MESSAGE_TYPE_NAME(target));

  gstd_iformatter_set_member_name (object->formatter,"source");
  gstd_iformatter_set_string_value (object->formatter, GST_MESSAGE_SRC_NAME(target));

  ts = g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS(target->timestamp));
  gstd_iformatter_set_member_name (object->formatter,"timestamp");
  gstd_iformatter_set_string_value (object->formatter, ts);
  g_free (ts);

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, target->seqnum);
  gstd_iformatter_set_member_name (object->formatter,"seqnum");
  gstd_iformatter_set_value (object->formatter, &value);
  g_value_unset (&value);

  error = NULL;
  gst_message_parse_error (target, &error, &debug);

  gstd_iformatter_set_member_name (object->formatter, "message");
  gstd_iformatter_set_string_value (object->formatter, error->message);

  gstd_iformatter_set_member_name (object->formatter, "debug");
  gstd_iformatter_set_string_value (object->formatter, debug);

  g_error_free (error);
  g_free (debug);

  gstd_iformatter_end_object (object->formatter);

  gstd_iformatter_generate (object->formatter, outstring);

  return GSTD_EOK;
}
