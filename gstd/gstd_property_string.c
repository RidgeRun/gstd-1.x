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

#include "gstd_property_string.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_property_string_debug);
#define GST_CAT_DEFAULT gstd_property_string_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyString, gstd_property_string, GSTD_TYPE_PROPERTY)

/* VTable */
static void
gstd_property_string_add_value (GstdProperty * self, GstdIFormatter *formatter,
    GValue * value);

static void
gstd_property_string_class_init (GstdPropertyStringClass *klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);

  pclass->add_value = GST_DEBUG_FUNCPTR(gstd_property_string_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_string_debug, "gstdpropertystring", debug_color,
			   "Gstd Property String category");

}

static void
gstd_property_string_init (GstdPropertyString *self)
{
  GST_INFO_OBJECT(self, "Initializing property string");
}

static void
gstd_property_string_add_value (GstdProperty * self, GstdIFormatter *formatter,
    GValue * value)
{
  const gchar * vstring;

  g_return_if_fail (self);
  g_return_if_fail (formatter);
  g_return_if_fail (value);

  vstring = g_value_get_string (value);

  gstd_iformatter_set_member_value (formatter, vstring);
}
