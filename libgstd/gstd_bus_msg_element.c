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
