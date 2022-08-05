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

#include "gstd_property_enum.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_enum_debug);
#define GST_CAT_DEFAULT gstd_property_enum_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyEnum, gstd_property_enum, GSTD_TYPE_PROPERTY);

/* VTable */
static GstdReturnCode
gstd_property_enum_update (GstdObject * object, const gchar * arg);

static void
gstd_property_enum_class_init (GstdPropertyEnumClass * klass)
{
  guint debug_color;
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_enum_update);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_enum_debug, "gstdpropertyenum",
      debug_color, "Gstd Property Enum category");

}

static void
gstd_property_enum_init (GstdPropertyEnum * self)
{
  GST_INFO_OBJECT (self, "Initializing property enum");
}

static GstdReturnCode
gstd_property_enum_update (GstdObject * object, const gchar * value)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdPropertyEnum *self;
  GstdProperty *prop;
  GParamSpec *pspec;
  GEnumClass *c;
  GEnumValue *e;
  guint64 d;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  self = GSTD_PROPERTY_ENUM (object);
  prop = GSTD_PROPERTY (object);

  g_return_val_if_fail (self->type != G_TYPE_NONE, GSTD_MISSING_INITIALIZATION);

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (prop->target),
      GSTD_OBJECT_NAME (prop));

  c = g_type_class_ref (pspec->value_type);

  /* Try by name */
  e = g_enum_get_value_by_name (c, value);
  if (e) {
    goto out;
  }

  /* Try by nick */
  e = g_enum_get_value_by_nick (c, value);
  if (e) {
    goto out;
  }

  /* Try by integer */
  errno = 0;
  d = g_ascii_strtoll (value, NULL, 10);
  if (d || !errno) {
    g_object_set (prop->target, pspec->name, d, NULL);
    goto error;                 //Not an error
  }

  ret = GSTD_BAD_VALUE;
  goto error;

out:
  g_object_set (prop->target, pspec->name, e->value, NULL);

error:
  g_type_class_unref (c);
  return ret;
}
