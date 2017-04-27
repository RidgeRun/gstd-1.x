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

/* Gstd Bus Msg debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_bus_msg_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void gstd_bus_msg_dispose (GObject * object);

G_DEFINE_TYPE (GstdBusMsg, gstd_bus_msg, GSTD_TYPE_OBJECT)

static void
gstd_bus_msg_class_init (GstdBusMsgClass *klass)
{
  GObjectClass * oclass;
  guint debug_color;

  oclass = G_OBJECT_CLASS (klass);

  oclass->dispose = gstd_bus_msg_dispose;
  
  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_debug, "gstdbusmsg", debug_color,
			   "Gstd Bus Msg category");

}

static void
gstd_bus_msg_init (GstdBusMsg *self)
{
  GST_INFO_OBJECT(self, "Initializing bus message");
}

static void
gstd_bus_msg_dispose (GObject * object)
{
  GstdBusMsg * self;

  self = GSTD_BUS_MSG (object);

  if (self->target) {
    gst_message_unref (self->target);
    self->target = NULL;
  }
}

GstdBusMsg *
gstd_bus_msg_factory_make (GstMessage * target)
{
  GstdBusMsg * msg = NULL;
  GstMessageType type;

  g_return_val_if_fail (target, NULL);

  type = GST_MESSAGE_TYPE(target);
  
  switch (type) {
  case (GST_MESSAGE_ERROR):
  case (GST_MESSAGE_WARNING):
  case (GST_MESSAGE_INFO):
    //msg = g_object_new (GST_TYPE_BUS_MSG_INFO, NULL);
    break;
  default:
    g_return_val_if_reached (NULL);
    break;
  }

  if (msg) {
    msg->target = target;
  }

  return msg;
}
