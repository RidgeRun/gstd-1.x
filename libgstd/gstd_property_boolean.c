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

#include "gstd_return_codes.h"
#include "gstd_property_boolean.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_boolean_debug);
#define GST_CAT_DEFAULT gstd_property_boolean_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyBoolean, gstd_property_boolean, GSTD_TYPE_PROPERTY);

/* VTable */
static void
gstd_property_boolean_add_value (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value);
static GstdReturnCode gstd_property_boolean_update (GstdObject * object,
    const gchar * value);

static void
gstd_property_boolean_class_init (GstdPropertyBooleanClass * klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_boolean_update);
  pclass->add_value = GST_DEBUG_FUNCPTR (gstd_property_boolean_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_boolean_debug, "gstdpropertyboolean",
      debug_color, "Gstd Property Boolean category");

}

static void
gstd_property_boolean_init (GstdPropertyBoolean * self)
{
  GST_INFO_OBJECT (self, "Initializing property boolean");
}


static void
gstd_property_boolean_add_value (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value)
{
  gstd_iformatter_set_value (formatter, value);
}

static GstdReturnCode
gstd_property_boolean_update (GstdObject * object, const gchar * value)
{
  GstdProperty *prop;
  GstdReturnCode ret = GSTD_EOK;
  gboolean bvalue;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  prop = GSTD_PROPERTY (object);

  if (0 == g_ascii_strcasecmp (value, "true") ||
      0 == g_ascii_strcasecmp (value, "yes") || 0 == g_strcmp0 (value, "1")) {
    bvalue = TRUE;
  } else if (0 == g_ascii_strcasecmp (value, "false") ||
      0 == g_ascii_strcasecmp (value, "no") || 0 == g_strcmp0 (value, "0")) {
    bvalue = FALSE;
  } else {
    return GSTD_BAD_VALUE;
  }

  g_object_set (prop->target, GSTD_OBJECT_NAME (prop), bvalue, NULL);
  return ret;
}
