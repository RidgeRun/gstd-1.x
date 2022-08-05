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
