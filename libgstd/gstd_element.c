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

#include <gobject/gvaluecollector.h>
#include <gst/gst.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include "gstd_action.h"
#include "gstd_element.h"
#include "gstd_event_handler.h"
#include "gstd_iformatter.h"
#include "gstd_json_builder.h"
#include "gstd_list.h"
#include "gstd_list_reader.h"
#include "gstd_object.h"
#include "gstd_property_array.h"
#include "gstd_property_boolean.h"
#include "gstd_property_enum.h"
#include "gstd_property_int.h"
#include "gstd_property_reader.h"
#include "gstd_property_string.h"
#include "gstd_signal.h"
#include "gstd_signal_list.h"

enum
{
  PROP_GSTELEMENT = 1,
  PROP_EVENT,
  PROP_PROPERTIES,
  PROP_SIGNALS,
  PROP_ACTIONS,
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

    /**
   * The gstd event handler for this element
   */
  GstdEventHandler *event_handler;

  /*
   * The properties held by the element
   */
  GstdList *element_properties;

  /*
   * The signals held by the element
   */
  GstdList *element_signals;

  /*
   * The actions held by the element
   */
  GstdList *element_actions;
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
void gstd_element_properties_to_string (GstdElement * self,
    GstdIFormatter * formatter);
void gstd_element_signals_to_string (GstdElement * self,
    GstdIFormatter * formatter);
void gstd_element_actions_to_string (GstdElement * self,
    GstdIFormatter * formatter);
static GstdReturnCode gstd_element_fill_properties (GstdElement * self);
static GstdReturnCode gstd_element_fill_signals_and_actions (GstdElement *
    self);
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

  properties[PROP_SIGNALS] =
      g_param_spec_object ("signals",
      "Signals",
      "The signals of the element",
      GSTD_TYPE_LIST,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_ACTIONS] =
      g_param_spec_object ("actions",
      "Actions",
      "The actions of the element",
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

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));

  self->element_properties =
      GSTD_LIST (g_object_new (GSTD_TYPE_LIST, "name", "element_properties",
          "node-type", GSTD_TYPE_PROPERTY, "flags", GSTD_PARAM_READ, NULL));

  self->element_signals =
      GSTD_LIST (g_object_new (GSTD_TYPE_SIGNAL_LIST, "name", "element_signals",
          "node-type", GSTD_TYPE_SIGNAL, "flags", GSTD_PARAM_READ, NULL));

  self->element_actions =
      GSTD_LIST (g_object_new (GSTD_TYPE_LIST, "name", "element_actions",
          "node-type", GSTD_TYPE_ACTION, "flags", GSTD_PARAM_READ, NULL));

  gstd_object_set_reader (GSTD_OBJECT (self->element_properties),
      g_object_new (GSTD_TYPE_LIST_READER, NULL));

  gstd_object_set_reader (GSTD_OBJECT (self->element_signals),
      g_object_new (GSTD_TYPE_LIST_READER, NULL));

  gstd_object_set_reader (GSTD_OBJECT (self->element_actions),
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

  g_object_unref (self->element_properties);
  g_object_unref (self->element_signals);
  g_object_unref (self->element_actions);

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
    case PROP_SIGNALS:
      GST_DEBUG_OBJECT (self, "Returning signals %p", self->element_signals);
      g_value_set_object (value, self->element_signals);
      break;
    case PROP_ACTIONS:
      GST_DEBUG_OBJECT (self, "Returning actions %p", self->element_actions);
      g_value_set_object (value, self->element_actions);
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
      gstd_element_fill_signals_and_actions (self);
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
gstd_element_properties_to_string (GstdElement * self,
    GstdIFormatter * formatter)
{
  GList *list;
  GParamSpec *pspec;
  GValue value = G_VALUE_INIT;
  GValue flags = G_VALUE_INIT;
  const gchar *typename;
  gchar *sflags;

  g_return_if_fail (GSTD_IS_OBJECT (self));

  gstd_iformatter_set_member_name (formatter, "element_properties");
  gstd_iformatter_begin_array (formatter);

  list = self->element_properties->list;
  while (list) {
    GstdProperty *property = list->data;

    if (property->pspec)
      pspec = property->pspec;
    else
      pspec =
          g_object_class_find_property (G_OBJECT_GET_CLASS (property->target),
          GSTD_OBJECT_NAME (property));
    /* Describe each parameter using a structure */
    gstd_iformatter_begin_object (formatter);

    gstd_iformatter_set_member_name (formatter, "name");

    gstd_iformatter_set_string_value (formatter, GSTD_OBJECT_NAME (property));

    typename = g_type_name (pspec->value_type);

    g_value_init (&value, pspec->value_type);
    g_object_get_property (G_OBJECT (property->target), pspec->name, &value);

    gstd_iformatter_set_member_name (formatter, "value");
    gstd_iformatter_set_value (formatter, &value);

    gstd_iformatter_set_member_name (formatter, "param");
    /* Describe the parameter specs using a structure */
    gstd_iformatter_begin_object (formatter);

    g_value_unset (&value);

    g_value_init (&flags, GSTD_TYPE_PARAM_FLAGS);
    g_value_set_flags (&flags, pspec->flags);
    sflags = g_strdup_value_contents (&flags);
    g_value_unset (&flags);

    gstd_iformatter_set_member_name (formatter, "description");
    gstd_iformatter_set_string_value (formatter, pspec->_blurb);

    gstd_iformatter_set_member_name (formatter, "type");
    gstd_iformatter_set_string_value (formatter, typename);

    gstd_iformatter_set_member_name (formatter, "access");
    gstd_iformatter_set_string_value (formatter, sflags);

    /* Close parameter specs structure */
    gstd_iformatter_end_object (formatter);

    g_free (sflags);

    /* Close parameter structure */
    gstd_iformatter_end_object (formatter);

    list = list->next;
  }

  gstd_iformatter_end_array (formatter);

}

static void
gstd_element_signals_to_string_internal (GstdElement * self,
    GList * signal_list, GstdIFormatter * formatter)
{
  GSignalQuery query;
  guint j;
  const gchar *typename;

  gstd_iformatter_begin_array (formatter);

  while (signal_list) {
    guint signal_id;

    signal_id = g_signal_lookup (GSTD_OBJECT_NAME (signal_list->data),
        G_OBJECT_TYPE (self->element));
    g_signal_query (signal_id, &query);

    /* Describe each signal using a structure */
    gstd_iformatter_begin_object (formatter);

    gstd_iformatter_set_member_name (formatter, "name");
    gstd_iformatter_set_string_value (formatter, query.signal_name);

    gstd_iformatter_set_member_name (formatter, "arguments");
    gstd_iformatter_begin_array (formatter);
    for (j = 0; j < query.n_params; j++) {
      typename = g_type_name (query.param_types[j]);
      gstd_iformatter_set_string_value (formatter, typename);
    }
    gstd_iformatter_end_array (formatter);

    gstd_iformatter_set_member_name (formatter, "return");
    typename = g_type_name (query.return_type);
    gstd_iformatter_set_string_value (formatter, typename);

    /* Close signal structure */
    gstd_iformatter_end_object (formatter);

    signal_list = signal_list->next;
  }

  gstd_iformatter_end_array (formatter);
}

void
gstd_element_signals_to_string (GstdElement * self, GstdIFormatter * formatter)
{
  GList *signal_list;

  g_return_if_fail (GSTD_IS_OBJECT (self));

  gstd_iformatter_set_member_name (formatter, "element_signals");
  signal_list = self->element_signals->list;
  gstd_element_signals_to_string_internal (self, signal_list, formatter);
}

void
gstd_element_actions_to_string (GstdElement * self, GstdIFormatter * formatter)
{
  GList *action_list = NULL;

  g_return_if_fail (GSTD_IS_OBJECT (self));

  gstd_iformatter_set_member_name (formatter, "element_actions");
  action_list = self->element_actions->list;
  gstd_element_signals_to_string_internal (self, action_list, formatter);
}

void
gstd_element_internal_to_string (GstdElement * self, gchar ** outstring)
{
  GstdIFormatter *formatter = NULL;

  g_return_if_fail (GSTD_IS_OBJECT (self));

  formatter = g_object_new (GSTD_TYPE_JSON_BUILDER, NULL);
  gstd_iformatter_begin_object (formatter);

  gstd_element_properties_to_string (self, formatter);
  gstd_element_signals_to_string (self, formatter);
  gstd_element_actions_to_string (self, formatter);

  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  /* Free formatter */
  g_object_unref (formatter);
}

static GstdReturnCode
gstd_element_append_object_properties (GstObject * object,
    GstdList * properties, GstElement * target, gchar * property_suffix)
{
  GParamSpec **properties_array;
  guint n_properties;
  GstdObject *element_property;
  GType type;
  GObjectClass *g_class;
  guint i;
  gchar *property_name;

  g_return_val_if_fail (GST_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GST_IS_OBJECT (target), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (properties, GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (target, "Gathering \"%s\" properties",
      GST_OBJECT_NAME (object));

  g_class = G_OBJECT_GET_CLASS (object);
  properties_array = g_object_class_list_properties (g_class, &n_properties);

  for (i = 0; i < n_properties; ++i) {
    type = gstd_element_property_get_type (properties_array[i]->value_type);
    if (property_suffix)
      property_name =
          g_strconcat (property_suffix, properties_array[i]->name, NULL);
    else
      property_name = g_strdup (properties_array[i]->name);

    element_property = g_object_new (type, "name",
        property_name, "target", target, "pspec", properties_array[i], NULL);

    g_free (property_name);
    gstd_list_append_child (properties, element_property);
  }

  g_free (properties_array);

  return GSTD_EOK;
}

static GstdReturnCode
gstd_element_fill_child_properties (GstdElement * self, GstObject * element,
    gchar * hierarchy)
{
  GObject *child;
  guint count;
  gint i;
  gchar *suffix;

  if (!GST_IS_CHILD_PROXY (element))
    goto out;

  count = gst_child_proxy_get_children_count (GST_CHILD_PROXY (element));

  GST_DEBUG_OBJECT (self, "%s has %d childrens", GST_OBJECT_NAME (element),
      count);
  if (!count)
    goto out;

  for (i = 0; i < count; i++) {
    child = gst_child_proxy_get_child_by_index (GST_CHILD_PROXY (element), i);
    if (!child || !GST_IS_OBJECT (child))
      continue;

    if (hierarchy)
      suffix = g_strconcat (hierarchy, GST_OBJECT_NAME (child), "::", NULL);
    else
      suffix = g_strdup_printf ("%s::", GST_OBJECT_NAME (child));

    GST_DEBUG_OBJECT (self, "Child suffix %s", suffix);
    gstd_element_fill_child_properties (self, GST_OBJECT (child), suffix);

    gstd_element_append_object_properties (GST_OBJECT (child),
        self->element_properties, GST_ELEMENT_CAST (child), suffix);

    g_free (suffix);
    g_object_unref (child);
  }

out:
  return GSTD_EOK;
}

static GstdReturnCode
gstd_element_fill_properties (GstdElement * self)
{

  g_return_val_if_fail (GSTD_IS_ELEMENT (self), GSTD_NULL_ARGUMENT);

  gstd_element_append_object_properties (GST_OBJECT (self->element),
      self->element_properties, self->element, NULL);

  gstd_element_fill_child_properties (self, GST_OBJECT (self->element), NULL);
  return GSTD_EOK;
}


static GstdReturnCode
gstd_element_fill_signals_and_actions (GstdElement * self)
{
  guint *signals;
  guint n_signals;
  GSignalQuery query;
  GstdObject *gstd_object;
  GType type;
  guint i;

  g_return_val_if_fail (GSTD_IS_ELEMENT (self), GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Gathering \"%s\" signals & actions",
      GST_OBJECT_NAME (self->element));

  for (type = G_OBJECT_TYPE (self->element); type; type = g_type_parent (type)) {

    signals = g_signal_list_ids (type, &n_signals);

    for (i = 0; i < n_signals; ++i) {
      g_signal_query (signals[i], &query);

      GST_DEBUG_OBJECT (self, "signal/action query %d: name %s", i,
          query.signal_name);
      if (query.signal_flags & G_SIGNAL_ACTION) {
        gstd_object = g_object_new (GSTD_TYPE_ACTION, "name",
            query.signal_name, "target", self->element, NULL);
        gstd_list_append_child (self->element_actions, gstd_object);
      } else {
        gstd_object = g_object_new (GSTD_TYPE_SIGNAL, "name",
            query.signal_name, "target", self->element, NULL);
        gstd_list_append_child (self->element_signals, gstd_object);
      }
    }
    g_free (signals);
  }

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

  if (g_type == G_TYPE_ARRAY) {
    return GSTD_TYPE_PROPERTY_ARRAY;
  } else {
    return GSTD_TYPE_PROPERTY;
  }

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
