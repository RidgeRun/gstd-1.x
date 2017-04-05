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

#include "gstd_property.h"

enum {
    PROP_TARGET = 1,
    N_PROPERTIES
};

#define DEFAULT_PROP_TARGET NULL

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_property_debug);
#define GST_CAT_DEFAULT gstd_property_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdProperty:
 * A wrapper for the conventional property
 */

G_DEFINE_TYPE (GstdProperty, gstd_property, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_property_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_property_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_property_dispose (GObject *);

static void
gstd_property_class_init (GstdPropertyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
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
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_READ);


  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);


  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_debug, "gstdproperty", debug_color,
			   "Gstd Property category");

}

static void
gstd_property_init (GstdProperty *self)
{
  GST_INFO_OBJECT(self, "Initializing property");
  self->target = DEFAULT_PROP_TARGET;
}

static void
gstd_property_dispose (GObject *object)
{
  GstdProperty *self = GSTD_PROPERTY(object);

  GST_INFO_OBJECT(self, "Disposing %s property", GSTD_OBJECT_NAME(self));

  if (self->target) {
    g_object_unref(self->target);
    self->target = NULL;
  }

  G_OBJECT_CLASS(gstd_property_parent_class)->dispose(object);
}

static void
gstd_property_get_property (GObject        *object,
			    guint           property_id,
			    GValue         *value,
			    GParamSpec     *pspec)
{
  GstdProperty *self = GSTD_PROPERTY(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);

  switch (property_id) {
  case PROP_TARGET:
    GST_DEBUG_OBJECT(self, "Returning property owner %p (%s)", self->target, GST_OBJECT_NAME(self->target));
    g_value_set_object (value, self->target);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_property_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  GstdProperty *self = GSTD_PROPERTY (object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);

  switch (property_id) {
  case PROP_TARGET:
    if (self->target)
      g_object_unref (self->target);
    self->target = g_value_dup_object (value);
    GST_DEBUG_OBJECT(self, "Setting property owner %p (%s)",self->target,
    		     GST_OBJECT_NAME(self->target));
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}
