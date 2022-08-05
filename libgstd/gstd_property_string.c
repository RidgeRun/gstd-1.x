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

#include "gstd_property_string.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_string_debug);
#define GST_CAT_DEFAULT gstd_property_string_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyString, gstd_property_string, GSTD_TYPE_PROPERTY);

/* VTable */
static void
gstd_property_string_add_value (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value);
static GstdReturnCode gstd_property_string_update (GstdObject * object,
    const gchar * arg);

static void
gstd_property_string_class_init (GstdPropertyStringClass * klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_string_update);
  pclass->add_value = GST_DEBUG_FUNCPTR (gstd_property_string_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_string_debug, "gstdpropertystring",
      debug_color, "Gstd Property String category");

}

static void
gstd_property_string_init (GstdPropertyString * self)
{
  GST_INFO_OBJECT (self, "Initializing property string");
}

static void
gstd_property_string_add_value (GstdProperty * self, GstdIFormatter * formatter,
    GValue * value)
{
  gstd_iformatter_set_value (formatter, value);
}

static GstdReturnCode
gstd_property_string_update (GstdObject * object, const gchar * value)
{
  GstdProperty *prop;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  prop = GSTD_PROPERTY (object);

  g_object_set (prop->target, GSTD_OBJECT_NAME (prop), value, NULL);

  return GSTD_EOK;
}
