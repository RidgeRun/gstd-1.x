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
#include <string.h>

#include "gstd_list.h"
#include "gstd_object.h"

enum
{
  PROP_COUNT = 1,
  PROP_NODE_TYPE,
  PROP_FLAGS,
  N_PROPERTIES                  // NOT A PROPERTY
};

#define GSTD_LIST_DEFAULT_COUNT 0
#define GSTD_LIST_DEFAULT_NODE_TYPE G_TYPE_NONE
#define GSTD_LIST_DEFAULT_FLAGS GSTD_PARAM_READ | GSTD_PARAM_CREATE | GSTD_PARAM_DELETE

/* Gstd List debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_list_debug);
#define GST_CAT_DEFAULT gstd_list_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/* VTable */
static gint gstd_list_find_node (gconstpointer, gconstpointer);
static GstdReturnCode
gstd_list_create (GstdObject * object, const gchar * name,
    const gchar * description);
static GstdReturnCode
gstd_list_delete (GstdObject * object, const gchar * name);
static GstdReturnCode gstd_list_to_string (GstdObject *, gchar **);

G_DEFINE_TYPE (GstdList, gstd_list, GSTD_TYPE_OBJECT);

/* VTable */
static void gstd_list_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_list_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_list_dispose (GObject *);

static void
gstd_list_class_init (GstdListClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstd_object_class = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_list_set_property;
  object_class->get_property = gstd_list_get_property;
  object_class->dispose = gstd_list_dispose;

  properties[PROP_COUNT] =
      g_param_spec_uint ("count",
      "Count",
      "The amount of nodes in the list",
      0, G_MAXINT, GSTD_LIST_DEFAULT_COUNT, G_PARAM_READABLE | GSTD_PARAM_READ);

  properties[PROP_NODE_TYPE] =
      g_param_spec_gtype ("node-type",
      "Node type",
      "The type of the node that the list holds",
      GSTD_LIST_DEFAULT_NODE_TYPE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | GSTD_PARAM_READ);

  properties[PROP_FLAGS] =
      g_param_spec_flags ("flags",
      "Flags",
      "The resource access flags",
      GSTD_TYPE_PARAM_FLAGS, GSTD_LIST_DEFAULT_FLAGS,
      //                      G_PARAM_CONSTRUCT_ONLY |
      G_PARAM_READWRITE | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gstd_object_class->create = gstd_list_create;
  gstd_object_class->delete = gstd_list_delete;
  gstd_object_class->to_string = gstd_list_to_string;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_list_debug, "gstdlist", debug_color,
      "Gstd List category");
}

static void
gstd_list_init (GstdList * self)
{
  GST_INFO_OBJECT (self, "Initializing list");
  self->list = NULL;
  self->count = GSTD_LIST_DEFAULT_COUNT;
  self->node_type = GSTD_LIST_DEFAULT_NODE_TYPE;
}

static void
gstd_list_dispose (GObject * object)
{
  GstdList *self = GSTD_LIST (object);

  GST_INFO_OBJECT (self, "Disposing %s list", GSTD_OBJECT_NAME (self));

  GST_OBJECT_LOCK (self);
  if (self->list) {
    g_list_free_full (self->list, g_object_unref);
    self->list = NULL;
  }
  GST_OBJECT_UNLOCK (self);

  G_OBJECT_CLASS (gstd_list_parent_class)->dispose (object);
}

static void
gstd_list_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdList *self = GSTD_LIST (object);

  switch (property_id) {
    case PROP_COUNT:
      GST_DEBUG_OBJECT (self, "Returning count of %u", self->count);
      g_value_set_uint (value, self->count);
      break;
    case PROP_NODE_TYPE:
      GST_DEBUG_OBJECT (self, "Returning type %s",
          g_type_name (self->node_type));
      g_value_set_gtype (value, self->node_type);
      break;
    case PROP_FLAGS:
      GST_DEBUG_OBJECT (self, "Returning flags %u", self->flags);
      g_value_set_flags (value, self->flags);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_list_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdList *self = GSTD_LIST (object);

  switch (property_id) {
    case PROP_NODE_TYPE:
      GST_DEBUG_OBJECT (self, "Setting node type to %s",
          g_type_name (self->node_type));
      self->node_type = g_value_get_gtype (value);
      break;
    case PROP_FLAGS:
      GST_DEBUG_OBJECT (self, "Setting node type to %u", self->flags);
      self->flags = g_value_get_flags (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static gint
gstd_list_find_node (gconstpointer _obj, gconstpointer _name)
{
  GstdObject *obj = GSTD_OBJECT (_obj);
  gchar *name = (gchar *) _name;

  GST_LOG ("Comparing %s vs %s", GSTD_OBJECT_NAME (obj), name);

  return strcmp (GSTD_OBJECT_NAME (obj), name);
}

static GstdReturnCode
gstd_list_create (GstdObject * object, const gchar * name,
    const gchar * description)
{
  GstdList *self;
  GstdObject *out;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);

  self = GSTD_LIST (object);

  g_return_val_if_fail (object->creator, GSTD_MISSING_INITIALIZATION);
  ret = gstd_icreator_create (object->creator, name, description, &out);
  if (ret) {
    goto error;
  }
  if (NULL == out) {
    ret = GSTD_BAD_COMMAND;
    goto error;
  }

  self->count++;

  if (!gstd_list_append_child (self, out)) {
    g_object_unref (out);
    ret = GSTD_EXISTING_RESOURCE;
    return ret;
  }

  return ret;
error:
  {
    if (out)
      g_object_unref (out);

    GST_ERROR_OBJECT (object, "Could not create the resource  \"%s\" on \"%s\"",
        name, GSTD_OBJECT_NAME (self));
    return ret;
  }
}


static GstdReturnCode
gstd_list_delete (GstdObject * object, const gchar * node)
{
  GstdList *self;
  GstdObject *todelete;
  GList *found;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (node, GSTD_NULL_ARGUMENT);

  self = GSTD_LIST (object);

  g_return_val_if_fail (object->deleter, GSTD_MISSING_INITIALIZATION);

  /* Test if the resource to delete exists */
  GST_OBJECT_LOCK (self);
  found = g_list_find_custom (self->list, node, gstd_list_find_node);

  if (!found) {
    GST_OBJECT_UNLOCK (self);
    goto unexisting;
  }

  todelete = GSTD_OBJECT (found->data);

  GST_INFO_OBJECT (self, "Deleting %s from %s list", GSTD_OBJECT_NAME (self),
      GSTD_OBJECT_NAME (self));

  ret = gstd_ideleter_delete (object->deleter, todelete);
  if (ret) {
    GST_OBJECT_UNLOCK (self);
    return ret;
  }

  self->count--;

  self->list = g_list_delete_link (self->list, found);
  GST_OBJECT_UNLOCK (self);

  return ret;

unexisting:
  {
    GST_ERROR_OBJECT (object, "The resource \"%s\" doesn't exists in \"%s\"",
        node, GSTD_OBJECT_NAME (self));
    return GSTD_NO_RESOURCE;
  }
}

static GstdReturnCode
gstd_list_to_string (GstdObject * object, gchar ** outstring)
{
  GstdList *self = GSTD_LIST (object);
  gchar *props;
  gchar *acc;
  gchar *node;
  GList *list;
  const gchar *separator;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  /* Lets leverage the parent's class implementation */
  GSTD_OBJECT_CLASS (gstd_list_parent_class)->to_string (GSTD_OBJECT (object),
      &props);
  // A little hack to remove the last bracket
  props[strlen (props) - 2] = '\0';

  list = self->list;
  acc = g_strdup ("");
  while (list) {
    separator = list->next ? "," : "";
    node =
        g_strdup_printf ("%s{\n    \"name\" : \"%s\"\n  }%s", acc,
        GSTD_OBJECT_NAME (list->data), separator);
    g_free (acc);
    acc = node;
    list = list->next;
  }

  *outstring = g_strdup_printf ("%s,\n  \"nodes\" : [%s]\n}", props, acc);
  g_free (props);
  g_free (acc);

  return GSTD_EOK;
}

GstdObject *
gstd_list_find_child (GstdList * self, const gchar * name)
{
  GList *result;
  GstdObject *child;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (name, NULL);

  GST_OBJECT_LOCK (self);
  result = g_list_find_custom (self->list, name, gstd_list_find_node);


  if (result) {
    child = GSTD_OBJECT (result->data);
  } else {
    child = NULL;
  }
  GST_OBJECT_UNLOCK (self);

  return child;
}

gboolean
gstd_list_append_child (GstdList * self, GstdObject * child)
{
  GList *found;

  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (child, GSTD_NULL_ARGUMENT);

  /* Test if the resource to create already exists */
  GST_OBJECT_LOCK (self);
  found =
      g_list_find_custom (self->list, GSTD_OBJECT_NAME (child),
      gstd_list_find_node);
  if (found) {
    GST_OBJECT_UNLOCK (self);
    goto exists;
  }

  self->list = g_list_append (self->list, child);
  self->count = g_list_length (self->list);
  GST_OBJECT_UNLOCK (self);
  GST_INFO_OBJECT (self, "Appended %s to %s list", GSTD_OBJECT_NAME (child),
      GSTD_OBJECT_NAME (self));

  return TRUE;

exists:
  {
    GST_ERROR_OBJECT (self, "The resource \"%s\" already exists in \"%s\"",
        GSTD_OBJECT_NAME (child), GSTD_OBJECT_NAME (self));
    return FALSE;
  }
}
