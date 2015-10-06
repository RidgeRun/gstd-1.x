/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
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

#include "gstd_list.h"
#include <gobject/gvaluecollector.h>

enum {
  PROP_COUNT = 1,
  PROP_NODE_TYPE,
  PROP_FLAGS,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_LIST_DEFAULT_COUNT 0
#define GSTD_LIST_DEFAULT_NODE_TYPE G_TYPE_NONE
#define GSTD_LIST_DEFAULT_FLAGS GSTD_PARAM_READ | GSTD_PARAM_CREATE | GSTD_PARAM_DELETE

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_list_debug);
#define GST_CAT_DEFAULT gstd_list_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

#define GSTD_TYPE_LIST_FLAGS (gstd_list_flags_get_type ())
static GType
gstd_list_flags_get_type (void)
{
  static GType list_flags_type = 0;
  static const GFlagsValue flags_types[] = {
    {GSTD_PARAM_CREATE, "CREATE", "create"},
    {GSTD_PARAM_READ, "READ", "read"},
    {GSTD_PARAM_UPDATE, "UPDATE", "update"},
    {GSTD_PARAM_DELETE, "DELETE", "delete"},
    {0, NULL, NULL}
  };
  if (!list_flags_type) {
    list_flags_type =
        g_flags_register_static ("GstdListFlags", flags_types);
  }
  return list_flags_type;
}

/* VTable */
static gint
gstd_list_find_node (gconstpointer, gconstpointer);
static
GstdReturnCode gstd_list_create (GstdObject *, const gchar *, va_list);
static
GstdReturnCode gstd_list_read (GstdObject *, const gchar *, va_list);
static
GstdReturnCode gstd_list_delete (GstdObject *, const gchar *);

/**
 * GstdList:
 * A wrapper for the conventional list
 */
struct _GstdList
{
  GstdObject parent;
  
  guint count;

  GType node_type;

  GParamFlags flags;
  
  GList *list;
};

G_DEFINE_TYPE (GstdList, gstd_list, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_list_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_list_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_list_dispose (GObject *);

static void
gstd_list_class_init (GstdListClass *klass)
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
		       0,
		       G_MAXINT,
		       GSTD_LIST_DEFAULT_COUNT,
		       G_PARAM_READABLE |
		       GSTD_PARAM_READ);

    properties[PROP_NODE_TYPE] =
    g_param_spec_gtype ("node-type",
		       "Node type",
		       "The type of the node that the list holds",
		       GSTD_LIST_DEFAULT_NODE_TYPE,
		       G_PARAM_CONSTRUCT_ONLY |
		       G_PARAM_READWRITE |
		       GSTD_PARAM_READ);

    properties[PROP_FLAGS] =
    g_param_spec_flags ("flags",
			"Flags",
			"The resource access flags",
			GSTD_TYPE_LIST_FLAGS,
			GSTD_LIST_DEFAULT_FLAGS,
			G_PARAM_CONSTRUCT_ONLY |
			G_PARAM_READWRITE |
			GSTD_PARAM_READ);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  gstd_object_class->create = gstd_list_create;
  gstd_object_class->read = gstd_list_read;
  gstd_object_class->delete = gstd_list_delete;
  
  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_list_debug, "gstdlist", debug_color,
			   "Gstd List category");
}

static void
gstd_list_init (GstdList *self)
{
  GST_INFO_OBJECT(self, "Initializing list");
  self->list = NULL;
  self->count = GSTD_LIST_DEFAULT_COUNT;
  self->node_type = GSTD_LIST_DEFAULT_NODE_TYPE;
}

static void
gstd_list_dispose (GObject *object)
{
  GstdList *self = GSTD_LIST(object);

  GST_INFO_OBJECT(self, "Disposing %s list", GSTD_OBJECT_NAME(self));

  if (self->list) {
    g_list_free_full(self->list, g_object_unref);
    self->list = NULL;
  }

  G_OBJECT_CLASS(gstd_list_parent_class)->dispose(object);
}

static void
gstd_list_get_property (GObject        *object,
			    guint           property_id,
			    GValue         *value,
			    GParamSpec     *pspec)
{
  GstdList *self = GSTD_LIST(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_COUNT:
    GST_DEBUG_OBJECT(self, "Returning count of %u", self->count);
    g_value_set_uint (value, self->count);
    break;
  case PROP_NODE_TYPE:
    GST_DEBUG_OBJECT(self, "Returning type %s",
		     g_type_name(self->node_type));
    g_value_set_gtype (value, self->node_type);
    break;
  case PROP_FLAGS:
    GST_DEBUG_OBJECT(self, "Returning flags %u", self->flags);
    g_value_set_flags (value, self->flags);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_list_set_property (GObject      *object,
			guint         property_id,
			const GValue *value,
			GParamSpec   *pspec)
{
  GstdList *self = GSTD_LIST (object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_NODE_TYPE:
    GST_DEBUG_OBJECT(self, "Setting node type to %s",
		     g_type_name(self->node_type));
    self->node_type = g_value_get_gtype (value);
    break;
  case PROP_FLAGS:
    GST_DEBUG_OBJECT(self, "Setting node type to %u", self->flags);
    self->flags = g_value_get_flags (value);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static gint
gstd_list_find_node (gconstpointer _obj, gconstpointer _name)
{
  GstdObject *obj = GSTD_OBJECT(_obj);
  gchar *name = (gchar*)_name;

  GST_LOG("Comparing %s vs %s", GSTD_OBJECT_NAME(obj),name);
  
  return strcmp(GSTD_OBJECT_NAME(obj), name);
}

static GstdReturnCode
gstd_list_create (GstdObject * object, const gchar *property, va_list va)
{
  GstdList *self = GSTD_LIST(object);
  GObject *newnode;
  GList *found;
  
  g_return_val_if_fail (GSTD_IS_LIST (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  /* Can we create resources on it */
  if (!GSTD_PARAM_IS_CREATE(self->flags))
    goto nocreate;
  
  /* Everything setup, create the new resource */
  newnode = g_object_new_valist (self->node_type, property, va);

  /* Test if the resource to create already exists */
  found = g_list_find_custom (self->list, GSTD_OBJECT_NAME(newnode),
			      gstd_list_find_node);
  if (found)
    goto exists;

  self->list = g_list_append (self->list, newnode);
  self->count = g_list_length (self->list);
  GST_INFO_OBJECT(self, "Appended %s to %s list", GSTD_OBJECT_NAME(newnode),
		  GSTD_OBJECT_NAME(self));
  
  return GSTD_EOK;

 nocreate:
  {
    GST_ERROR_OBJECT(object, "Cannot create resources in \"%s\"",
		     GSTD_OBJECT_NAME(self));
    return GSTD_NO_CREATE;
  }
 exists:
  {
    GST_ERROR_OBJECT(object, "The resource \"%s\" already exists in \"%s\"",
		     GSTD_OBJECT_NAME(newnode), GSTD_OBJECT_NAME(self));
    g_object_unref(newnode);
    return GSTD_EXISTING_RESOURCE;
  }
}

static GstdReturnCode
gstd_list_read (GstdObject * object, const gchar *property, va_list va)
{
  GstdList *self = GSTD_LIST(object);
  GList *found;
  const gchar *name;
  GstdReturnCode ret;
  GValue value = G_VALUE_INIT;
  gchar *error = NULL;

  g_return_val_if_fail (GSTD_IS_LIST (object), GSTD_NULL_ARGUMENT);

  /* Can we create resources on it */
  if (!GSTD_PARAM_IS_READ(self->flags))
    goto noread;

  ret = GSTD_EOK;
  name = property;
  
  while (name) {
    found = g_list_find_custom (self->list, name, gstd_list_find_node);
    if (!found)
      goto noexist;

    g_value_init (&value, self->node_type);
    g_value_set_object(&value, G_OBJECT(found->data));
    
    G_VALUE_LCOPY(&value, va, 0, &error);

    if (error) {
      GST_ERROR_OBJECT(self, "%s", error);
      g_free (error);
      g_value_unset (&value);
      ret |= GSTD_NO_CREATE;
    } else {
      GST_INFO_OBJECT(self, "Read object %s from %s", property,
		      GSTD_OBJECT_NAME(self));
    }

    g_value_unset (&value);
    name = va_arg (va, const gchar *);
  }
  
  return ret;

 noexist:
  {
    GST_ERROR_OBJECT(self, "The node %s does not exist in %s", property, GSTD_OBJECT_NAME(self));
    return GSTD_NO_CREATE;
  }
 noread:
  {
    GST_ERROR_OBJECT(self, "Cannot read from %s", GSTD_OBJECT_NAME(self));
    return GSTD_NO_CREATE;
  }
}

static GstdReturnCode
gstd_list_delete (GstdObject * object, const gchar *node)
{
  GstdList *self = GSTD_LIST(object);
  GList *found;
  GstdObject *todelete;
  
  g_return_val_if_fail (GSTD_IS_LIST (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (node, GSTD_NULL_ARGUMENT);

  /* Can we create resources on it */
  if (!GSTD_PARAM_IS_DELETE(self->flags))
    goto nodelete;

  found = g_list_find_custom (self->list, node, gstd_list_find_node);
  if (!found)
    goto unexisting;
  todelete = GSTD_OBJECT(found->data);

  GST_INFO_OBJECT(self, "Deleting %s from %s list", GSTD_OBJECT_NAME(self),
		   GSTD_OBJECT_NAME(self));
  self->list = g_list_delete_link (self->list, found);
  g_object_unref(todelete);
  
  return GSTD_EOK;

 nodelete:
  {
    GST_ERROR_OBJECT(object, "Cannot delete resources from \"%s\"",
		     GSTD_OBJECT_NAME(self));
    return GSTD_NO_CREATE;
  }
 unexisting:
  {
    GST_ERROR_OBJECT(object, "The resource \"%s\" doesn't exists in \"%s\"",
		     node, GSTD_OBJECT_NAME(self));
    return GSTD_EXISTING_RESOURCE;
  }
}
