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
GstdReturnCode
gstd_property_boolean_to_string (GstdObject * self, gchar ** outstring);

static void
gstd_property_boolean_class_init (GstdPropertyBooleanClass *klass)
{
  guint debug_color;
  GstdObjectClass *gstd_object_class = GSTD_OBJECT_CLASS (klass);

  gstd_object_class->to_string = gstd_property_boolean_to_string;

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

GstdReturnCode
gstd_property_boolean_to_string (GstdObject * self, gchar ** outstring)
{
  GParamSpec * property;
  gboolean value;
  gchar * svalue;
  gchar *sflags;
  GValue flags = G_VALUE_INIT;
  const gchar *typename;

  GstdProperty * prop;
  prop = GSTD_PROPERTY(self);

  g_return_val_if_fail (GSTD_IS_OBJECT (self), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  property = g_object_class_find_property(G_OBJECT_GET_CLASS(prop->target), GSTD_OBJECT_NAME(self));

  /* Describe each parameter using a structure */
  gstd_iformatter_begin_object (self->formatter);

  gstd_iformatter_set_member_name (self->formatter,"name");

  gstd_iformatter_set_member_value (self->formatter, property->name);

  g_object_get(G_OBJECT(prop->target), property->name, &value, NULL);

  svalue = g_strdup_printf ("%s", value ? "TRUE":"FALSE");

  gstd_iformatter_set_member_name (self->formatter,"value");
  gstd_iformatter_set_member_value (self->formatter, svalue);

  gstd_iformatter_set_member_name (self->formatter, "param_spec");
  /* Describe the parameter specs using a structure */
  gstd_iformatter_begin_object (self->formatter);

  g_value_init (&flags, GSTD_TYPE_PARAM_FLAGS);
  g_value_set_flags (&flags, property->flags);
  sflags = g_strdup_value_contents(&flags);
  g_value_unset(&flags);

  gstd_iformatter_set_member_name (self->formatter, "blurb");
  gstd_iformatter_set_member_value (self->formatter,property->_blurb);

  gstd_iformatter_set_member_name (self->formatter, "type");
  gstd_iformatter_set_member_value (self->formatter,typename);

  gstd_iformatter_set_member_name (self->formatter, "access");
  gstd_iformatter_set_member_value (self->formatter,sflags);

  gstd_iformatter_set_member_name (self->formatter, "construct");
  gstd_iformatter_set_member_value (self->formatter,GSTD_PARAM_IS_DELETE(property->flags) ? "TRUE" : "FALSE");
  /* Close parameter specs structure */
  gstd_iformatter_end_object (self->formatter);

  g_free (sflags);
  g_free (svalue);
  /* Close parameter structure */
  gstd_iformatter_end_object (self->formatter);

  gstd_iformatter_generate (self->formatter, outstring);

  return GSTD_EOK;
}
