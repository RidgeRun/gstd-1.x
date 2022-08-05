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

#include "gstd_property_int.h"

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_int_debug);
#define GST_CAT_DEFAULT gstd_property_int_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdPropertyInt, gstd_property_int, GSTD_TYPE_PROPERTY);

/* VTable */
static void
gstd_property_int_add_value (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value);
static GstdReturnCode gstd_property_int_update (GstdObject * object,
    const gchar * arg);

static void
gstd_property_int_class_init (GstdPropertyIntClass * klass)
{
  guint debug_color;
  GstdPropertyClass *pclass = GSTD_PROPERTY_CLASS (klass);
  GstdObjectClass *oclass = GSTD_OBJECT_CLASS (klass);

  oclass->update = GST_DEBUG_FUNCPTR (gstd_property_int_update);
  pclass->add_value = GST_DEBUG_FUNCPTR (gstd_property_int_add_value);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_int_debug, "gstdpropertyint",
      debug_color, "Gstd Property Int category");

}

static void
gstd_property_int_init (GstdPropertyInt * self)
{
  GST_INFO_OBJECT (self, "Initializing property int");
}

static void
gstd_property_int_add_value (GstdProperty * self, GstdIFormatter * formatter,
    GValue * value)
{
  gstd_iformatter_set_value (formatter, value);
}

static GstdReturnCode
gstd_property_int_update (GstdObject * object, const gchar * value)
{
  GstdProperty *prop;
  GParamSpec *pspec;
  GstdReturnCode ret = GSTD_EOK;
  gint64 parsed;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  prop = GSTD_PROPERTY (object);

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (prop->target),
      GSTD_OBJECT_NAME (prop));

  g_return_val_if_fail (pspec, GSTD_MISSING_INITIALIZATION);

  errno = 0;

  switch (pspec->value_type) {
    case G_TYPE_INT64:
    case G_TYPE_INT:
    {
      parsed = g_ascii_strtoll (value, NULL, 10);
      if (!parsed && errno) {
        GST_ERROR_OBJECT (object, "Cannot update %s: %s", pspec->name,
            g_strerror (errno));
        ret = GSTD_BAD_VALUE;
        goto out;
      }
      break;
    }
    case G_TYPE_UINT64:
    case G_TYPE_UINT:
    {
      parsed = g_ascii_strtoull (value, NULL, 10);
      if (!parsed && errno) {
        GST_ERROR_OBJECT (object, "Cannot update %s: %s", pspec->name,
            g_strerror (errno));
        ret = GSTD_BAD_VALUE;
        goto out;
      }
      break;
    }
    default:
      g_warn_if_reached ();
      goto out;
  }

  g_object_set (prop->target, GSTD_OBJECT_NAME (prop), parsed, NULL);

out:
  {
    return ret;
  }

}
