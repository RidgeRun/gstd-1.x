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
#include "gstd_bus_msg_notify.h"

/* Gstd Bus MsgNotify debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_notify_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_notify_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_NOTIFY

static GstdReturnCode
gstd_bus_msg_notify_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target);

struct _GstdBusMsgNotify
{
  GstdBusMsg parent;
};

struct _GstdBusMsgNotifyClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgNotify, gstd_bus_msg_notify, GSTD_TYPE_BUS_MSG);

static void
gstd_bus_msg_notify_class_init (GstdBusMsgNotifyClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_notify_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_notify_debug, "gstdbusmsgnotify",
      debug_color, "Gstd Bus Msg Notify category");

}

static void
gstd_bus_msg_notify_init (GstdBusMsgNotify * self)
{
  GST_INFO_OBJECT (self, "Initializing bus property-notify message");
}

static GstdReturnCode
gstd_bus_msg_notify_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  GstdReturnCode ret = GSTD_BAD_COMMAND;

#if GST_VERSION_MINOR >= 10
  GstObject *object = NULL;
  const gchar *property_name = NULL;
  const GValue *property_value = NULL;
  gchar *object_name = NULL;
  gchar *property_value_str = NULL;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target->type == GST_MESSAGE_PROPERTY_NOTIFY,
      GSTD_EVENT_ERROR);

  gst_message_parse_property_notify (target, &object, &property_name,
      &property_value);

  object_name = gst_object_get_path_string (GST_OBJECT (object));

  if (property_value != NULL) {
    property_value_str = g_strdup_value_contents (property_value);
  } else {
    property_value_str = g_strdup ("(no value)");
  }

  gstd_iformatter_set_member_name (formatter, "object-name");
  gstd_iformatter_set_string_value (formatter, object_name);

  gstd_iformatter_set_member_name (formatter, "property_name");
  gstd_iformatter_set_string_value (formatter, property_name);

  gstd_iformatter_set_member_name (formatter, "message");
  gstd_iformatter_set_string_value (formatter, property_value_str);

  if (object_name) {
    g_free (object_name);
  }
  if (property_value_str) {
    g_free (property_value_str);
  }
#endif

  return ret;
}
