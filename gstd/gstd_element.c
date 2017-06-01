/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <gst/gst.h>
#include <gobject/gvaluecollector.h>
#include <json-glib/json-glib.h>

#include "gstd_list.h"
#include "gstd_element.h"
#include "gstd_object.h"
#include "gstd_event_handler.h"

#include "gstd_iformatter.h"
#include "gstd_json_builder.h"
#include "gstd_property_reader.h"
#include "gstd_property_boolean.h"
#include "gstd_property_string.h"
#include "gstd_property_int.h"
#include "gstd_property_enum.h"
#include "gstd_list_reader.h"

enum
{
  PROP_GSTELEMENT = 1,
  PROP_EVENT,
  PROP_PROPERTIES,
  N_PROPERTIES                  // NOT A PROPERTY
};

#define GSTD_ELEMENT_DEFAULT_GSTELEMENT NULL

/* Gstd Session debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_element_debug);
#define GST_CAT_DEFAULT gstd_element_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdElement:
 * A wrapper for the conventional element
 */
struct _GstdElement
{
  GstdObject parent;

  /**
   * A Gstreamer element holding the element
   */
  GstElement *element;

  GstdIFormatter *formatter;
    /**
   * The gstd event handler for this element
   */
  GstdEventHandler *event_handler;

  /*
   * The properties held by the element
   */
  GstdList *element_properties;

};

struct _GstdElementClass
{
  GstdObjectClass parent_class;
};


G_DEFINE_TYPE (GstdElement, gstd_element, GSTD_TYPE_OBJECT);

/* VTable */
static void
gstd_element_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_element_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_element_dispose (GObject *);
static GstdReturnCode gstd_element_to_string (GstdObject *, gchar **);
void gstd_element_internal_to_string (GstdElement *, gchar **);
static GstdReturnCode gstd_element_fill_properties (GstdElement * self);
static GType gstd_element_property_get_type (GType g_type);
static void
gstd_element_class_init (GstdElementClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstd_object_class = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_element_set_property;
  object_class->get_property = gstd_element_get_property;
  object_class->dispose = gstd_element_dispose;

  properties[PROP_GSTELEMENT] =
      g_param_spec_object ("gstelement",
      "GstElement",
      "The internal Gstreamer element",
      GST_TYPE_ELEMENT,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_EVENT] =
      g_param_spec_object ("event", "Event",
      "The event handler of the element",
      GSTD_TYPE_EVENT_HANDLER, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_PROPERTIES] =
      g_param_spec_object ("properties",
      "Properties",
      "The properties of the element",
      GSTD_TYPE_LIST,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gstd_object_class->to_string = gstd_element_to_string;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_element_debug, "gstdelement", debug_color,
      "Gstd Element category");

}

static void
gstd_element_init (GstdElement * self)
{
  GST_INFO_OBJECT (self, "Initializing element");
  self->element = GSTD_ELEMENT_DEFAULT_GSTELEMENT;
  self->event_handler = NULL;
  self->formatter = g_object_new (GSTD_TYPE_JSON_BUILDER, NULL);

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));

  self->element_properties =
      GSTD_LIST (g_object_new (GSTD_TYPE_LIST, "name", "element_properties",
          "node-type", GSTD_TYPE_PROPERTY, "flags", GSTD_PARAM_READ, NULL));

  gstd_object_set_reader (GSTD_OBJECT (self->element_properties),
      g_object_new (GSTD_TYPE_LIST_READER, NULL));
}

static void
gstd_element_dispose (GObject * object)
{
  GstdElement *self = GSTD_ELEMENT (object);

  GST_INFO_OBJECT (self, "Disposing %s element", GSTD_OBJECT_NAME (self));

  if (self->element) {
    g_object_unref (self->element);
    self->element = NULL;
  }

  if (self->event_handler) {
    g_object_unref (self->event_handler);
    self->event_handler = NULL;
  }

  /* Free formatter */
  g_object_unref (self->formatter);
  g_object_unref (self->element_properties);

  G_OBJECT_CLASS (gstd_element_parent_class)->dispose (object);
}

static void
gstd_element_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdElement *self = GSTD_ELEMENT (object);

  switch (property_id) {
    case PROP_GSTELEMENT:
      GST_DEBUG_OBJECT (self, "Returning gstelement %p (%s)", self->element,
          GST_OBJECT_NAME (self->element));
      g_value_set_object (value, self->element);
      break;
    case PROP_EVENT:
      GST_DEBUG_OBJECT (self, "Returning event handler %p",
          self->event_handler);
      g_value_set_object (value, self->event_handler);
      break;
    case PROP_PROPERTIES:
      GST_DEBUG_OBJECT (self, "Returning properties %p",
          self->element_properties);
      g_value_set_object (value, self->element_properties);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      return;
  }
}

static void
gstd_element_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdElement *self = GSTD_ELEMENT (object);

  switch (property_id) {
    case PROP_GSTELEMENT:
      self->element = g_object_ref (g_value_get_object (value));
      if (self->event_handler) {
        g_object_unref (self->event_handler);
      }
      self->event_handler = g_object_new (GSTD_TYPE_EVENT_HANDLER, "receiver",
          G_OBJECT (self->element), NULL);

      GST_DEBUG_OBJECT (self, "Setting element %p (%s)", self->element,
          GST_OBJECT_NAME (self->element));

      gstd_element_fill_properties (self);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static GstdReturnCode
gstd_element_to_string (GstdObject * object, gchar ** outstring)
{
  GstdElement *self = GSTD_ELEMENT (object);
  gchar *props;
  gchar *internal;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  /* Lets leverage the parent's class implementation */
  GSTD_OBJECT_CLASS (gstd_element_parent_class)->to_string (GSTD_OBJECT
      (object), &props);
  // A little hack to remove the last bracket
  props[strlen (props) - 2] = '\0';

  // Now parse the properties of the internal GST element
  gstd_element_internal_to_string (self, &internal);

  *outstring = g_strdup_printf ("%s,%s", props, internal);
  g_free (props);
  g_free (internal);

  return GSTD_EOK;
}

void
gstd_element_internal_to_string (GstdElement * self, gchar ** outstring)
{
  GParamSpec **properties;
  GValue value = G_VALUE_INIT;
  GValue flags = G_VALUE_INIT;
  gchar *sflags;
  guint n, i;
  const gchar *typename;

  g_return_if_fail (GSTD_IS_OBJECT (self));

  gstd_iformatter_begin_object (self->formatter);
  gstd_iformatter_set_member_name (self->formatter, "element_properties");
  gstd_iformatter_begin_array (self->formatter);

  properties =
      g_object_class_list_properties (G_OBJECT_GET_CLASS (self->element), &n);
  for (i = 0; i < n; i++) {
    /* Describe each parameter using a structure */
    gstd_iformatter_begin_object (self->formatter);

    gstd_iformatter_set_member_name (self->formatter, "name");

    gstd_iformatter_set_string_value (self->formatter, properties[i]->name);

    typename = g_type_name (properties[i]->value_type);

    g_value_init (&value, properties[i]->value_type);
    g_object_get_property (G_OBJECT (self->element), properties[i]->name,
        &value);

    gstd_iformatter_set_member_name (self->formatter, "value");
    gstd_iformatter_set_value (self->formatter, &value);

    gstd_iformatter_set_member_name (self->formatter, "param");
    /* Describe the parameter specs using a structure */
    gstd_iformatter_begin_object (self->formatter);

    g_value_unset (&value);

    g_value_init (&flags, GSTD_TYPE_PARAM_FLAGS);
    g_value_set_flags (&flags, properties[i]->flags);
    sflags = g_strdup_value_contents (&flags);
    g_value_unset (&flags);

    gstd_iformatter_set_member_name (self->formatter, "description");
    gstd_iformatter_set_string_value (self->formatter, properties[i]->_blurb);

    gstd_iformatter_set_member_name (self->formatter, "type");
    gstd_iformatter_set_string_value (self->formatter, typename);

    gstd_iformatter_set_member_name (self->formatter, "access");
    gstd_iformatter_set_string_value (self->formatter, sflags);

    /* Close parameter specs structure */
    gstd_iformatter_end_object (self->formatter);

    g_free (sflags);

    /* Close parameter structure */
    gstd_iformatter_end_object (self->formatter);
  }
  g_free (properties);

  gstd_iformatter_end_array (self->formatter);
  gstd_iformatter_end_object (self->formatter);

  gstd_iformatter_generate (self->formatter, outstring);
}

static GstdReturnCode
gstd_element_fill_properties (GstdElement * self)
{
  GParamSpec **properties_array;
  guint n_properties;
  GstdObject *element_property;
  GType type;
  GObjectClass *g_class;
  guint i;
  g_class = G_OBJECT_GET_CLASS (self->element);

  g_return_val_if_fail (GSTD_IS_ELEMENT (self), GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Gathering \"%s\" properties",
      GST_OBJECT_NAME (self));

  properties_array = g_object_class_list_properties (g_class, &n_properties);

  for (i = 0; i < n_properties; ++i) {
    type = gstd_element_property_get_type (properties_array[i]->value_type);
    element_property = g_object_new (type, "name",
        properties_array[i]->name, "target", self->element, NULL);

    gstd_list_append_child (self->element_properties, element_property);
  }

  g_free (properties_array);

  return GSTD_EOK;
}

static GType
gstd_element_property_get_type (GType g_type)
{

  //FIXME:
  //I just found a way to handle all types in a generic way, hence,
  //the base property class can handle them all. I don't want to remove
  //specific type sublasses because the to_string method may require to
  //add details. For example, int properties can display their max and min
  //values, flags and enums could display the options, etc... Similar to
  //what gst-inspect does
  return GSTD_TYPE_PROPERTY;

  switch (g_type) {
    case G_TYPE_BOOLEAN:
    {
      return GSTD_TYPE_PROPERTY_BOOLEAN;
    }
    case G_TYPE_INT:
    case G_TYPE_UINT:
    case G_TYPE_UINT64:
    case G_TYPE_INT64:
    {
      return GSTD_TYPE_PROPERTY_INT;
    }
    case G_TYPE_STRING:
    {
      return GSTD_TYPE_PROPERTY_STRING;
    }
    default:
    {
      if (G_TYPE_IS_ENUM (g_type)) {
        return GSTD_TYPE_PROPERTY_ENUM;
      } else {
        return GSTD_TYPE_PROPERTY;
      }
    }
  }
}
