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

#include "gstd_property_flags.h"
#include "gstd_msg_type.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_flags_debug);
#define GST_CAT_DEFAULT gstd_property_flags_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyFlags, gstd_property_flags, GSTD_TYPE_PROPERTY);

/* VTable */
static GstdReturnCode
gstd_property_flags_update (GstdObject * object, const gchar * arg);

static void
gstd_property_flags_class_init (GstdPropertyFlagsClass * klass)
{
  guint debug_color;
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_flags_update);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_flags_debug, "gstdpropertyflags",
      debug_color, "Gstd Property Flags category");

}

static void
gstd_property_flags_init (GstdPropertyFlags * self)
{
  GST_INFO_OBJECT (self, "Initializing property flags");
}

static GstdReturnCode
gstd_property_flags_update (GstdObject * object, const gchar * svalue)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdPropertyFlags *self;
  GstdProperty *prop;
  GParamSpec *pspec;
  GValue value = G_VALUE_INIT;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (svalue, GSTD_NULL_ARGUMENT);

  self = GSTD_PROPERTY_FLAGS (object);
  prop = GSTD_PROPERTY (object);

  g_return_val_if_fail (self->type != G_TYPE_NONE, GSTD_MISSING_INITIALIZATION);

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (prop->target),
      GSTD_OBJECT_NAME (prop));

  g_value_init (&value, pspec->value_type);

  if (!gst_value_deserialize (&value, svalue)) {
    ret = GSTD_BAD_VALUE;
  } else {
    g_object_set_property (prop->target, pspec->name, &value);
    ret = GSTD_EOK;
  }

  return ret;
}
