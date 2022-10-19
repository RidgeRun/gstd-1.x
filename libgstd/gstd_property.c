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

#include "gstd_property.h"

enum
{
  PROP_TARGET = 1,
  PROP_PSPEC,
  N_PROPERTIES
};

#define DEFAULT_PROP_TARGET NULL
#define DEFAULT_PROP_PSPEC  NULL

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_property_debug);
#define GST_CAT_DEFAULT gstd_property_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdProperty:
 * A wrapper for the conventional property
 */

G_DEFINE_TYPE (GstdProperty, gstd_property, GSTD_TYPE_OBJECT);

/* VTable */
static void
gstd_property_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_property_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_property_dispose (GObject *);
static GstdReturnCode
gstd_property_to_string (GstdObject * obj, gchar ** outstring);
static void
gstd_property_add_value_default (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value);
static GstdReturnCode gstd_property_update_default (GstdObject * object,
    const gchar * arg);

static void
gstd_property_class_init (GstdPropertyClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstdc = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_property_set_property;
  object_class->get_property = gstd_property_get_property;
  object_class->dispose = gstd_property_dispose;

  properties[PROP_TARGET] =
      g_param_spec_object ("target",
      "Target",
      "The target object owning the property",
      G_TYPE_OBJECT,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_PSPEC] =
      g_param_spec_pointer ("pspec",
      "Property Specification",
      "The property meta-specification associated with the property",
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gstdc->to_string = GST_DEBUG_FUNCPTR (gstd_property_to_string);
  gstdc->update = GST_DEBUG_FUNCPTR (gstd_property_update_default);

  klass->add_value = GST_DEBUG_FUNCPTR (gstd_property_add_value_default);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_debug, "gstdproperty", debug_color,
      "Gstd Property category");

}

static void
gstd_property_init (GstdProperty * self)
{
  GST_INFO_OBJECT (self, "Initializing property");
  self->target = DEFAULT_PROP_TARGET;
  self->pspec = DEFAULT_PROP_PSPEC;
}

static void
gstd_property_dispose (GObject * object)
{
  GstdProperty *self = GSTD_PROPERTY (object);

  GST_INFO_OBJECT (self, "Disposing %s property", GSTD_OBJECT_NAME (self));

  if (self->target) {
    g_object_unref (self->target);
    self->target = NULL;
  }

  self->pspec = NULL;
  G_OBJECT_CLASS (gstd_property_parent_class)->dispose (object);
}

static void
gstd_property_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdProperty *self = GSTD_PROPERTY (object);

  switch (property_id) {
    case PROP_TARGET:
      GST_DEBUG_OBJECT (self, "Returning property owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      g_value_set_object (value, self->target);
      break;
    case PROP_PSPEC:
      GST_DEBUG_OBJECT (self, "Returning property spec %p", self->pspec);
      g_value_set_pointer (value, self->pspec);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_property_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdProperty *self = GSTD_PROPERTY (object);

  switch (property_id) {
    case PROP_TARGET:
      if (self->target)
        g_object_unref (self->target);
      self->target = g_value_dup_object (value);
      GST_DEBUG_OBJECT (self, "Setting property owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      break;
    case PROP_PSPEC:
      self->pspec = g_value_get_pointer (value);
      GST_DEBUG_OBJECT (self, "Setting property spec %p", self->pspec);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static GstdReturnCode
gstd_property_to_string (GstdObject * obj, gchar ** outstring)
{
  GParamSpec *property;
  GstdProperty *self;
  GstdPropertyClass *klass;
  GValue value = G_VALUE_INIT;
  gchar *sflags;
  const gchar *typename;
  GstdIFormatter *formatter = g_object_new (obj->formatter_factory, NULL);

  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_PROPERTY (obj);
  klass = GSTD_PROPERTY_GET_CLASS (self);

  GST_OBJECT_LOCK (self);

  if (self->pspec)
    property = self->pspec;
  else
    property = g_object_class_find_property (G_OBJECT_GET_CLASS (self->target),
        GSTD_OBJECT_NAME (self));

  /* Describe each parameter using a structure */
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "name");
  gstd_iformatter_set_string_value (formatter, property->name);

  if (property->flags & G_PARAM_READABLE) {
    gstd_iformatter_set_member_name (formatter, "value");

    g_value_init (&value, property->value_type);
    g_object_get_property (G_OBJECT (self->target), property->name, &value);

    g_assert (klass->add_value);
    klass->add_value (self, formatter, &value);

    g_value_unset (&value);
  } else {
    gstd_iformatter_set_member_name (formatter, "value");
    gstd_iformatter_set_null_value (formatter);
  }

  gstd_iformatter_set_member_name (formatter, "param");
  /* Describe the parameter specs using a structure */
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "description");
  gstd_iformatter_set_string_value (formatter, property->_blurb);

  typename = g_type_name (property->value_type);
  gstd_iformatter_set_member_name (formatter, "type");
  gstd_iformatter_set_string_value (formatter, typename);

  g_value_init (&value, GSTD_TYPE_PARAM_FLAGS);
  g_value_set_flags (&value, property->flags);
  sflags = g_strdup_value_contents (&value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "access");
  gstd_iformatter_set_string_value (formatter, sflags);

  g_free (sflags);

  /* Close parameter specs structure */
  gstd_iformatter_end_object (formatter);

  /* Close parameter structure */
  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  GST_OBJECT_UNLOCK (self);

  /* Free formatter */
  g_object_unref (formatter);
  return GSTD_EOK;
}

static void
gstd_property_add_value_default (GstdProperty * self,
    GstdIFormatter * formatter, GValue * value)
{
  gchar *svalue;

  g_return_if_fail (self);
  g_return_if_fail (formatter);
  g_return_if_fail (value);

  svalue = gst_value_serialize (value);
  gstd_iformatter_set_string_value (formatter, svalue);
  g_free (svalue);
}

static GstdReturnCode
gstd_property_update_default (GstdObject * object, const gchar * svalue)
{
  GstdReturnCode ret;
  GParamSpec *pspec;
  GstdProperty *prop;
  GValue value = G_VALUE_INIT;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (svalue, GSTD_NULL_ARGUMENT);

  prop = GSTD_PROPERTY (object);

  GST_OBJECT_LOCK (prop);

  if (prop->pspec) {
    pspec = prop->pspec;
  } else {
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (prop->target),
        GSTD_OBJECT_NAME (prop));
  }

  g_value_init (&value, pspec->value_type);

  if (!gst_value_deserialize (&value, svalue)) {
    ret = GSTD_BAD_VALUE;
  } else {
    g_object_set_property (prop->target, pspec->name, &value);
    ret = GSTD_EOK;
  }

  g_value_unset (&value);

  GST_OBJECT_UNLOCK (prop);

  return ret;
}
