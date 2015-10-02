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

enum {
  PROP_PROPERTIES = 1,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_ELEMENT_DEFAULT_PROPERTIES NULL

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

static void
gstd_element_class_init (GstdElementClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_element_set_property;
  object_class->get_property = gstd_element_get_property;
  object_class->dispose = gstd_element_dispose;

  properties[PROP_PROPERTIES] =
    g_param_spec_object ("properties",
			 "Properties",
			 "The properties of the element",
			 GST_TYPE_ELEMENT,
			 G_PARAM_READWRITE |
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_READ |
			 GSTD_PARAM_UPDATE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_element_debug, "gstdelement", debug_color,
			   "Gstd Element category");
}

static void
gstd_element_init (GstdElement *self)
{
  GST_INFO_OBJECT(self, "Initializing element");
  self->element = GSTD_ELEMENT_DEFAULT_PROPERTIES;
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
  case PROP_PROPERTIES:
    GST_DEBUG_OBJECT(self, "Returning properties %p (%s)", self->element,
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
  case PROP_PROPERTIES:
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
