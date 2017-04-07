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

#include "gstd_property_boolean.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_property_boolean_debug);
#define GST_CAT_DEFAULT gstd_property_boolean_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyBoolean, gstd_property_boolean, GSTD_TYPE_PROPERTY)

/* VTable */
static void
gstd_property_boolean_add_value (GstdProperty * self, GstdIFormatter *formatter,
    GValue * value);

static void
gstd_property_boolean_class_init (GstdPropertyBooleanClass *klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);

  pclass->add_value = GST_DEBUG_FUNCPTR(gstd_property_boolean_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_boolean_debug, "gstdpropertyboolean", debug_color,
			   "Gstd Property Boolean category");

}

static void
gstd_property_boolean_init (GstdPropertyBoolean *self)
{
  GST_INFO_OBJECT(self, "Initializing property boolean");
}


static void
gstd_property_boolean_add_value (GstdProperty * self, GstdIFormatter *formatter,
    GValue * value)
{
  gstd_iformatter_set_value (formatter, value);
}
