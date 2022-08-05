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
