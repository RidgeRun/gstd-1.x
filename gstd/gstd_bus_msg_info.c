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

static void
gstd_bus_msg_info_class_init (GstdBusMsgInfoClass *klass)
{
  GstdBusMsgClass * bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR(gstd_bus_msg_info_to_string);

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
gstd_bus_msg_info_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  GError * error;
  gchar * debug;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  error = NULL;
  gst_message_parse_error (target, &error, &debug);

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
