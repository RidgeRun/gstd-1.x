/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_bus_msg.h"
#include "gstd_bus_msg_info.h"

/* Gstd Bus MsgInfo debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_info_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_info_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_bus_msg_info_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target);

struct _GstdBusMsgInfo
{
  GstdBusMsg parent;
};

struct _GstdBusMsgInfoClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgInfo, gstd_bus_msg_info, GSTD_TYPE_BUS_MSG)

static void gstd_bus_msg_info_class_init (GstdBusMsgInfoClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_info_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_info_debug, "gstdbusmsginfo",
      debug_color, "Gstd Bus Msg Info category");

}

static void
gstd_bus_msg_info_init (GstdBusMsgInfo * self)
{
  GST_INFO_OBJECT (self, "Initializing bus info/error/warning message");
}

static GstdReturnCode
gstd_bus_msg_info_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  GError *error;
  gchar *debug;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  error = NULL;
  switch(target->type) {
  case GST_MESSAGE_ERROR:
    gst_message_parse_error (target, &error, &debug);
    break;
  case GST_MESSAGE_WARNING:
    gst_message_parse_warning (target, &error, &debug);
    break;
  case GST_MESSAGE_INFO:
    gst_message_parse_info (target, &error, &debug);
    break;
  case GST_MESSAGE_CLOCK_PROVIDE:
    gst_message_parse_clock_provide (target, &error, &debug);
    break;
  case GST_MESSAGE_STREAM_STATUS:
    gst_message_parse_stream_status (target, &error, &debug);
    break;
  case GST_MESSAGE_SEGMENT_DONE:
    gst_message_parse_segment_done (target, &error, &debug);
    break;
  case GST_MESSAGE_SEGMENT_START:
    gst_message_parse_segment_start (target, &error, &debug);
    break;
  case GST_MESSAGE_TOC:
    gst_message_parse_toc (target, &error, &debug);
    break;
#if GST_VERSION_MINOR >= 16
  case GST_MESSAGE_DEVICE_CHANGED:
    gst_message_parse_device_changed (target, &error, &debug);
    break;
#endif
#if GST_VERSION_MINOR >= 18
  case GST_MESSAGE_INSTANT_RATE_REQUEST:
    gst_message_parse_instant_state_request (target, &error, &debug);
    break;
#endif
  /* This messages don't match the current format  */
  case GST_MESSAGE_TAG:
  case GST_MESSAGE_STATE_CHANGED:
  case GST_MESSAGE_STEP_DONE:
  case GST_MESSAGE_CLOCK_LOST:
  case GST_MESSAGE_NEW_CLOCK:
  case GST_MESSAGE_ASYNC_DONE:
  case GST_MESSAGE_REQUEST_STATE:
  case GST_MESSAGE_STEP_START:
  case GST_MESSAGE_QOS:
  case GST_MESSAGE_PROGRESS:
  case GST_MESSAGE_RESET_TIME:
  /* This messages don't have their own parser */
  case GST_MESSAGE_EOS:
  case GST_MESSAGE_BUFFERING:
  case GST_MESSAGE_STATE_DIRTY:
  case GST_MESSAGE_APPLICATION:
  case GST_MESSAGE_ELEMENT:
  case GST_MESSAGE_DURATION_CHANGED:
  case GST_MESSAGE_LATENCY:
  case GST_MESSAGE_ASYNC_START:
  case GST_MESSAGE_STREAM_START:
#if GST_VERSION_MINOR >= 2
  case GST_MESSAGE_NEED_CONTEXT:
  case GST_MESSAGE_HAVE_CONTEXT:
#endif
#if GST_VERSION_MINOR >= 4
  case GST_MESSAGE_DEVICE_ADDED:
  case GST_MESSAGE_DEVICE_REMOVED:
  case GST_MESSAGE_EXTENDED:
#endif
#if GST_VERSION_MINOR >= 10
  case GST_MESSAGE_PROPERTY_NOTIFY:
  case GST_MESSAGE_STREAM_COLLECTION:
  case GST_MESSAGE_STREAMS_SELECTED:
#endif
  case GST_MESSAGE_UNKNOWN:
  case GST_MESSAGE_ANY:    
    break;
  default:
    return GSTD_EVENT_ERROR;
  }

  if(error) {
    gstd_iformatter_set_member_name (formatter, "message");
    gstd_iformatter_set_string_value (formatter, error->message);

    gstd_iformatter_set_member_name (formatter, "debug");
    gstd_iformatter_set_string_value (formatter, debug);

    g_error_free (error);
  }
  else {
    gstd_iformatter_set_member_name (formatter, "message");
    gstd_iformatter_set_string_value (formatter, "");

    gstd_iformatter_set_member_name (formatter, "debug");
    gstd_iformatter_set_string_value (formatter, "");
  }

  if (debug) {
    g_free (debug);
  }

  return GSTD_EOK;
}
