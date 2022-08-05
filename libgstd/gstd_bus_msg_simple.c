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
#include "gstd_bus_msg_simple.h"

/* Gstd Bus MsgSimple debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_simple_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_simple_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_SIMPLE

static GstdReturnCode
gstd_bus_msg_simple_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target);

struct _GstdBusMsgSimple
{
  GstdBusMsg parent;
};

struct _GstdBusMsgSimpleClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgSimple, gstd_bus_msg_simple, GSTD_TYPE_BUS_MSG);

static void
gstd_bus_msg_simple_class_init (GstdBusMsgSimpleClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_simple_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_simple_debug, "gstdbusmsgsimple",
      debug_color, "Gstd Bus Msg Simple category");

}

static void
gstd_bus_msg_simple_init (GstdBusMsgSimple * self)
{
  GST_INFO_OBJECT (self, "Initializing bus info/error/warning message");
}

static GstdReturnCode
gstd_bus_msg_simple_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  GError *error;
  gchar *debug;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  error = NULL;
  switch (target->type) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error (target, &error, &debug);
      break;
    case GST_MESSAGE_WARNING:
      gst_message_parse_warning (target, &error, &debug);
      break;
    case GST_MESSAGE_INFO:
      gst_message_parse_info (target, &error, &debug);
      break;
    default:
      return GSTD_EVENT_ERROR;
  }

  gstd_iformatter_set_member_name (formatter, "message");
  gstd_iformatter_set_string_value (formatter, error->message);

  gstd_iformatter_set_member_name (formatter, "debug");
  gstd_iformatter_set_string_value (formatter, debug);

  g_error_free (error);

  if (debug) {
    g_free (debug);
  }

  return GSTD_EOK;
}
