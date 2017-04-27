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

#include "gstd_property_flags.h"
#include "gstd_msg_type.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_property_flags_debug);
#define GST_CAT_DEFAULT gstd_property_flags_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyFlags, gstd_property_flags, GSTD_TYPE_PROPERTY)

/* VTable */
static GstdReturnCode
gstd_property_flags_update (GstdObject * object, const gchar * arg);

static void
gstd_property_flags_class_init (GstdPropertyFlagsClass *klass)
{
  guint debug_color;
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR(gstd_property_flags_update);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_flags_debug, "gstdpropertyflags", debug_color,
			   "Gstd Property Flags category");

}

static void
gstd_property_flags_init (GstdPropertyFlags *self)
{
  GST_INFO_OBJECT(self, "Initializing property flags");
}

static GstdReturnCode
gstd_property_flags_update (GstdObject * object, const gchar * value)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdPropertyFlags * self;
  GstdProperty * prop;
  GParamSpec *pspec;
  GValue flags = G_VALUE_INIT;
  guint types;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  self = GSTD_PROPERTY_FLAGS (object);
  prop = GSTD_PROPERTY (object);

  g_return_val_if_fail (self->type != G_TYPE_NONE, GSTD_MISSING_INITIALIZATION);

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(prop->target),
      GSTD_OBJECT_NAME(prop));

  g_value_init(&flags, pspec->value_type);

  gst_value_deserialize (&flags, value);

  types = g_value_get_flags (&flags);
  if (GST_MESSAGE_UNKNOWN == types) {
    ret = GSTD_BAD_VALUE;
  } else {
    g_object_set (prop->target, pspec->name, types, NULL);
    ret = GSTD_EOK;
  }

  return ret;
}
