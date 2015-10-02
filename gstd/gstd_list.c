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

enum {
  PROP_COUNT = 1,
  PROP_ELEMENT_TYPE,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_LIST_DEFAULT_COUNT 0
#define GSTD_LIST_DEFAULT_ELEMENT_TYPE G_TYPE_NONE

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_list_debug);
#define GST_CAT_DEFAULT gstd_list_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/* VTable */
static gint
gstd_list_find_element (gconstpointer, gconstpointer);

/**
 * GstdList:
 * A wrapper for the conventional list
 */
struct _GstdList
{
  GstdObject parent;
  
  guint count;

  GType element_type;
  
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
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_list_set_property;
  object_class->get_property = gstd_list_get_property;
  object_class->dispose = gstd_list_dispose;

  properties[PROP_COUNT] =
    g_param_spec_uint ("count",
		       "Count",
		       "The amount of elements in the list",
		       0,
		       G_MAXINT,
		       GSTD_LIST_DEFAULT_COUNT,
		       G_PARAM_READABLE |
		       GSTD_PARAM_READ);

    properties[PROP_ELEMENT_TYPE] =
    g_param_spec_gtype ("element-type",
		       "Element type",
		       "The type of the element that the list holds",
		       GSTD_LIST_DEFAULT_ELEMENT_TYPE,
		       G_PARAM_CONSTRUCT_ONLY |
		       G_PARAM_READWRITE |
		       GSTD_PARAM_READ);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

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
  self->element_type = GSTD_LIST_DEFAULT_ELEMENT_TYPE;
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
  case PROP_ELEMENT_TYPE:
    GST_DEBUG_OBJECT(self, "Returning type %s",
		     g_type_name(self->element_type));
    g_value_set_gtype (value, self->element_type);
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
  case PROP_ELEMENT_TYPE:
    GST_DEBUG_OBJECT(self, "Setting element type to %s",
		     g_type_name(self->element_type));
    self->element_type = g_value_get_gtype (value);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static gint
gstd_list_find_element (gconstpointer _obj, gconstpointer _name)
{
  GstdObject *obj = GSTD_OBJECT(_obj);
  gchar *name = (gchar*)_name;

  GST_LOG("Comparing %s vs %s", GSTD_OBJECT_NAME(obj),name);
  
  return strcmp(GSTD_OBJECT_NAME(obj), name);
}

GstdReturnCode
gstd_list_append (GstdList *self, GstdObject *object)
{
  GList *found;
  
  g_return_val_if_fail (GSTD_IS_LIST(self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT(self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (G_OBJECT_TYPE(object) == self->element_type,
			GSTD_NULL_ARGUMENT);

  /* Test if the resource to create already exists */
  found = g_list_find_custom (self->list, GSTD_OBJECT_NAME(object),
			      gstd_list_find_element);
  if (found)
    goto exists;
  
  self->list = g_list_append (self->list, object);

  return GSTD_EOK;
  
 exists:
  {
    return GSTD_EXISTING_RESOURCE;
  }
}
