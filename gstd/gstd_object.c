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
#include <stdarg.h>
#include "gstd_object.h"

enum {
  PROP_NAME = 1,
  N_PROPERTIES // NOT A PROPERTY
};

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_object_debug);
#define GST_CAT_DEFAULT gstd_object_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdObject, gstd_object, G_TYPE_OBJECT)

/* VTable */
static void
gstd_object_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_object_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_object_dispose (GObject *);
static gint
gstd_object_find_resource (gconstpointer, gconstpointer);

static void
gstd_object_class_init (GstdObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_object_set_property;
  object_class->get_property = gstd_object_get_property;
  object_class->dispose = gstd_object_dispose;

  properties[PROP_NAME] =
    g_param_spec_string ("name",
			 "Name",
			 "The name of the current Gstd session",
			 GSTD_OBJECT_DEFAULT_NAME,
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 G_PARAM_READWRITE |
			 GSTD_PARAM_READ
			 );
  
  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);
  
  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_object_debug, "gstdobject", debug_color,
			   "Gstd Object category");
}

static void
gstd_object_init (GstdObject *self)
{
  GST_DEBUG_OBJECT(self, "Initializing gstd object");

  self->name = g_strdup(GSTD_OBJECT_DEFAULT_NAME);
}

static void
gstd_object_get_property (GObject        *object,
		   guint           property_id,
		   GValue         *value,
		   GParamSpec     *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);

  switch (property_id) {
  case PROP_NAME:
    GST_DEBUG_OBJECT(self, "Returning object name \"%s\"", self->name);
    g_value_set_string (value, self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gstd_object_set_property (GObject      *object,
		   guint         property_id,
		   const GValue *value,
		   GParamSpec   *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  switch (property_id) {
  case PROP_NAME:
    if (self->name)
      g_free(self->name);
    
    self->name = g_value_dup_string (value);
    GST_INFO_OBJECT(self, "Changed object name to %s", self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
gstd_object_dispose (GObject *object)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  GST_DEBUG_OBJECT(object, "Deinitializing gstd object");

  if (self->name) {
    g_free (self->name);
    self->name = NULL;
  }
  
  G_OBJECT_CLASS(gstd_object_parent_class)->dispose(object);
}

static gint
gstd_object_find_resource (gconstpointer _obj, gconstpointer _name)
{
  GstdObject *obj = GSTD_OBJECT(_obj);
  gchar *name = (gchar*)_name;

  GST_LOG("Comparing %s vs %s", GSTD_OBJECT_NAME(obj),name);
  
  return strcmp(GSTD_OBJECT_NAME(obj), name);
}

GstdReturnCode
gstd_object_create (GstdObject *object, const gchar *property, ...)
{
  va_list ap;
  GParamSpec *spec;
  GList *list, *found;
  GstdObject *nnew;
  GType *resourcetype;
  const gchar *first;

  g_return_val_if_fail (G_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  /* Does the property exist? */
  spec = g_object_class_find_property (G_OBJECT_GET_CLASS(object), property);
  if (!spec)
    goto noproperty;

  /* Can we create resources on it */
  if (!GSTD_PARAM_IS_CREATE(spec->flags))
    goto nocreate;

  /* Assert since this is a programming error */
  g_return_val_if_fail(G_IS_PARAM_SPEC_POINTER(spec),
		       GSTD_NO_CREATE);

  /* Hack to get the type of the resource in the list */
  resourcetype = g_param_spec_get_qdata (spec,
      g_quark_from_static_string("ResourceType"));
  
  /* All validated to create the new resource */
  va_start(ap, property);
  first = va_arg(ap, gchar*);

  nnew = GSTD_OBJECT(g_object_new_valist (*resourcetype, first, ap));
  va_end(ap);

  list = NULL;
  g_object_get (object, property, &list, NULL);
  found = g_list_find_custom (list, GSTD_OBJECT_NAME(nnew),
			      gstd_object_find_resource);
  if (found)
    goto exists;

  /* Append it to the list of resources */
  list = g_list_append (list, nnew);
  g_object_set (object, property, list, NULL);
  
  return GSTD_EOK;

 noproperty:
  {
    GST_ERROR_OBJECT(object, "The property \"%s\" doesn't exist", property);
    return GSTD_NO_RESOURCE;
  }
 nocreate:
  {
    GST_ERROR_OBJECT(object, "Cannot create resources in \"%s\"", property);
    return GSTD_NO_CREATE;
  }
 exists:
  {
    GST_ERROR_OBJECT(object, "The resource \"%s\" already exists in \"%s\"",
		     GSTD_OBJECT_NAME(nnew), property);
    g_object_unref(nnew);
    return GSTD_EXISTING_RESOURCE;
  }
}

GstdReturnCode
gstd_object_read (GstdObject *object, const gchar *property, ...);

GstdReturnCode
gstd_object_update (GstdObject *object, const gchar *property, ...)
{
  va_list ap;
  GParamSpec *spec;
  GObject *propobject;
  const gchar *first;
  
  g_return_val_if_fail (G_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  /* Does the property exist? */
  spec = g_object_class_find_property (G_OBJECT_GET_CLASS(object), property);
  if (!spec)
    goto noproperty;

  /* Can we create resources on it */
  if (!GSTD_PARAM_IS_UPDATE(spec->flags))
    goto noupdate;

  /* Assert since this is a programming error */
  g_return_val_if_fail(G_IS_PARAM_SPEC_OBJECT(spec),
		       GSTD_NO_UPDATE);

  g_object_get(object, property, &propobject, NULL);

  va_start(ap, property);
  first = va_arg(ap, gchar*);
  
  g_object_set_valist (propobject, first, ap);
  va_end(ap);
		       
  g_object_unref(propobject);
    
  return GSTD_EOK;
  
 noproperty:
  {
    GST_ERROR_OBJECT(object, "The property \"%s\" doesn't exist", property);
    return GSTD_NO_RESOURCE;
  }
 noupdate:
  {
    GST_ERROR_OBJECT(object, "Cannot update resources in \"%s\"", property);
    return GSTD_NO_CREATE;
  }
}

GstdReturnCode
gstd_object_delete (GstdObject *object, const gchar *property, ...);

