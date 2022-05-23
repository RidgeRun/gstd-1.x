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

#include "gstd_property_array.h"
#include <stdlib.h>

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_array_debug);
#define GST_CAT_DEFAULT gstd_property_array_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyArray, gstd_property_array, GSTD_TYPE_PROPERTY);

/* VTable */
static void
gstd_property_array_add_value (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value);
static GstdReturnCode gstd_property_array_update (GstdObject * object,
    const gchar * arg);

static void
gstd_property_array_class_init (GstdPropertyArrayClass * klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_array_update);
  pclass->add_value = GST_DEBUG_FUNCPTR (gstd_property_array_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_array_debug, "gstdpropertyarray",
      debug_color, "Gstd Property Array category");

}

static void
gstd_property_array_init (GstdPropertyArray * self)
{
  GST_INFO_OBJECT (self, "Initializing property array");
}

static void
gstd_property_array_add_value (GstdProperty * self, GstdIFormatter * formatter,
    GValue * value)
{
  GArray *garray;
  gfloat fvalue;
  gint i;
  GValue val = G_VALUE_INIT;

  g_return_if_fail (self);
  g_return_if_fail (formatter);
  g_return_if_fail (value);

  g_value_init (&val, G_TYPE_FLOAT);
  garray = (GArray *) g_value_get_boxed (value);

  gstd_iformatter_begin_array (formatter);
  for (i = 0; i < garray->len; i++) {
    fvalue = g_array_index (garray, gfloat, i);
    g_value_set_float (&val, fvalue);
    gstd_iformatter_set_value (formatter, &val);
  }
  g_value_unset (&val);
  gstd_iformatter_end_array (formatter);
}

static GstdReturnCode
gstd_property_array_update (GstdObject * object, const gchar * value)
{
  GstdProperty *prop;
  GParamSpec *pspec;
  GstdReturnCode ret = GSTD_EOK;
  GArray *garray;
  gchar **tokens;
  gchar **token_counter;
  gchar *token;
  gfloat f_token;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  prop = GSTD_PROPERTY (object);

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (prop->target),
      GSTD_OBJECT_NAME (prop));

  g_return_val_if_fail (pspec, GSTD_MISSING_INITIALIZATION);

  errno = 0;

  garray = g_array_new (FALSE, FALSE, sizeof (gfloat));

  tokens = g_strsplit (value, " ", -1);
  if (!*tokens && errno) {
    GST_ERROR_OBJECT (object, "Cannot update %s: %s", pspec->name,
        g_strerror (errno));
    ret = GSTD_BAD_VALUE;
    goto out;
  }
  token_counter = tokens;

  while (*token_counter) {
    token = *token_counter++;
    f_token = atof (token);
    g_array_append_val (garray, f_token);
  }
  if (garray != NULL) {
    g_object_set (prop->target, GSTD_OBJECT_NAME (prop), garray, NULL);
  } else {
    GST_ERROR_OBJECT (object, "Cannot update %s: Array is empty", pspec->name);
    ret = GSTD_BAD_VALUE;
    goto out;
  }

out:
  {
    g_free (tokens);
    return ret;
  }
}
