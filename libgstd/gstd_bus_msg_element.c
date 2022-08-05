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

/* Gstd Bus MsgElement debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_bus_msg_element_debug);

#define GST_CAT_DEFAULT gstd_bus_msg_element_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode
gstd_bus_msg_element_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target);

struct _GstdBusMsgElement
{
  GstdBusMsg parent;
};

struct _GstdBusMsgElementClass
{
  GstdBusMsgClass parent_class;
};

G_DEFINE_TYPE (GstdBusMsgElement, gstd_bus_msg_element, GSTD_TYPE_BUS_MSG);

static void
gstd_bus_msg_element_class_init (GstdBusMsgElementClass * klass)
{
  GstdBusMsgClass *bmclass;
  guint debug_color;

  bmclass = GSTD_BUS_MSG_CLASS (klass);

  bmclass->to_string = GST_DEBUG_FUNCPTR (gstd_bus_msg_element_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_bus_msg_element_debug, "gstdbusmsgelement",
      debug_color, "Gstd Bus Msg Element category");

}

static void
gstd_bus_msg_element_init (GstdBusMsgElement * self)
{
  GST_INFO_OBJECT (self, "Initializing bus ELEMENT message");
}

static GstdReturnCode
gstd_bus_msg_element_to_string (GstdBusMsg * msg, GstdIFormatter * formatter,
    GstMessage * target)
{
  const GstStructure *st;
  gint field;

  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (formatter, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (target, GSTD_NULL_ARGUMENT);

  st = gst_message_get_structure (target);

  /* The structure is not mandatory */
  if (NULL == st) {
    goto out;
  }

  gstd_iformatter_set_member_name (formatter, gst_structure_get_name (st));
  gstd_iformatter_begin_object (formatter);

  /* Iterate through all fields in the structure */
  for (field = 0; field < gst_structure_n_fields (st); ++field) {
    const gchar *field_name;
    const GValue *field_value;

    field_name = gst_structure_nth_field_name (st, field);
    field_value = gst_structure_get_value (st, field_name);

    gstd_iformatter_set_member_name (formatter, field_name);
    gstd_iformatter_set_value (formatter, field_value);
  }

  gstd_iformatter_end_object (formatter);

out:
  return GSTD_EOK;
}
