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
#include "gstd_element.h"
#include <string.h>
#include <gobject/gvaluecollector.h>

enum {
  PROP_GSTELEMENT = 1,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_ELEMENT_DEFAULT_GSTELEMENT NULL

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_element_debug);
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
};

G_DEFINE_TYPE (GstdElement, gstd_element, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_element_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_element_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_element_dispose (GObject *);
static GstdReturnCode
gstd_element_read (GstdObject *, const gchar*, va_list);
static GstdReturnCode
gstd_element_update (GstdObject *, const gchar*, va_list);

static void
gstd_element_class_init (GstdElementClass *klass)
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
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_READ);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  gstd_object_class->read = gstd_element_read;
  gstd_object_class->update = gstd_element_update;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_element_debug, "gstdelement", debug_color,
			   "Gstd Element category");
}

static void
gstd_element_init (GstdElement *self)
{
  GST_INFO_OBJECT(self, "Initializing element");
  self->element = GSTD_ELEMENT_DEFAULT_GSTELEMENT;
}

static void
gstd_element_dispose (GObject *object)
{
  GstdElement *self = GSTD_ELEMENT(object);

  GST_INFO_OBJECT(self, "Disposing %s element", GSTD_OBJECT_NAME(self));

  if (self->element) {
    g_object_unref(self->element);
    self->element = NULL;
  }

  G_OBJECT_CLASS(gstd_element_parent_class)->dispose(object);
}

static void
gstd_element_get_property (GObject        *object,
			    guint           property_id,
			    GValue         *value,
			    GParamSpec     *pspec)
{
  GstdElement *self = GSTD_ELEMENT(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_GSTELEMENT:
    GST_DEBUG_OBJECT(self, "Returning gstelement %p (%s)", self->element,
		     GST_OBJECT_NAME(self->element));
    g_value_set_object (value, self->element);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    return;
  }
}

static void
gstd_element_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  GstdElement *self = GSTD_ELEMENT (object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_GSTELEMENT:
    self->element = g_object_ref(g_value_get_object (value));
    GST_DEBUG_OBJECT(self, "Setting element %p (%s)",self->element,
		     GST_OBJECT_NAME(self->element));
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static GstdReturnCode
gstd_element_read (GstdObject *object, const gchar *property,
		   va_list va)
{
  GstdElement *self = GSTD_ELEMENT(object);
  GParamSpec *pspec;
  const gchar *name;
  GstdReturnCode ret;
  GValue value = G_VALUE_INIT;
  gchar *error = NULL;
  GObject *toset;
  
  g_return_val_if_fail (GSTD_IS_ELEMENT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  name = property;
  ret = GSTD_EOK;

  while (name) {
    // First look for the property in the container
    toset = G_OBJECT(self);
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(self),
					  name);
    if (!pspec) {
      pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(self->element),
					    name);
      toset = G_OBJECT(self->element);
    }
    
    if (!pspec) {
      GST_ERROR_OBJECT (self, "The property %s is not a property in %s",
			name, GSTD_OBJECT_NAME(self));
      ret |= GSTD_NO_CREATE;
      break;
    } 

    if (!GSTD_PARAM_IS_READ(pspec->flags)) {
      GST_ERROR_OBJECT (self, "The property %s is not readable", name);
      ret |= GSTD_NO_READ;
      break;
    }
    
    g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
    g_object_get_property (toset, name, &value);
    
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
}

static GstdReturnCode
gstd_element_update (GstdObject *object, const gchar *property,
		     va_list va)
{
  GstdElement *self = GSTD_ELEMENT(object);
  GParamSpec *pspec;
  const gchar *name;
  GstdReturnCode ret;
  GValue value = G_VALUE_INIT;
  gchar *error = NULL;
  
  g_return_val_if_fail (GSTD_IS_ELEMENT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  name = property;
  ret = GSTD_EOK;

  while (name) {
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(self->element),
					  name);
    if (!pspec) {
      GST_ERROR_OBJECT (self, "The property %s is not a property in %s",
			name, GSTD_OBJECT_NAME(self));
      ret |= GSTD_NO_UPDATE;
      break;
    } 

    if (pspec->flags & G_PARAM_WRITABLE & !G_PARAM_CONSTRUCT_ONLY) {
      GST_ERROR_OBJECT (self, "The property %s is not writable", name);
      ret |= GSTD_NO_UPDATE;
      break;
    }
    
    g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
    G_VALUE_COLLECT(&value, va, 0, &error);
    if (error) {
      GST_ERROR_OBJECT(self, "%s", error);
      g_free (error);
      g_value_unset (&value);
      ret |= GSTD_NO_CREATE;
    } else {
      g_object_set_property (G_OBJECT(self->element), name, &value);
      GST_INFO_OBJECT(self, "Wrote object %s from %s", property,
		      GSTD_OBJECT_NAME(self));
    }

    g_value_unset (&value);
    name = va_arg (va, const gchar *);  
  }
  
  return ret;
}
