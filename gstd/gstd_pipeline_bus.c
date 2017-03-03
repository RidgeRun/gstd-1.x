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

#include "gstd_pipeline_bus.h"


enum
{
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdPipelineBus
{
  GstdObject parent;

};

struct _GstdPipelineBusClass
{
  GstdObjectClass parent_class;
};

static void
gstd_pipeline_bus_set_property (GObject *,
    guint, const GValue *, GParamSpec *);

G_DEFINE_TYPE (GstdPipelineBus, gstd_pipeline_bus, GSTD_TYPE_OBJECT);

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_bus_debug);
#define GST_CAT_DEFAULT gstd_pipeline_bus_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void
gstd_pipeline_bus_class_init (GstdPipelineBusClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->set_property = gstd_pipeline_bus_set_property;


  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_bus_debug, "gstdpipelinebus",
      debug_color, "Gstd Pipeline Bus messages category");
}

static void
gstd_pipeline_bus_init (GstdPipelineBus * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd pipeline bus handler");
}


GstdPipelineBus *
gstd_pipeline_bus_new (GObject * receiver)
{
  return GSTD_PIPELINE_BUS (g_object_new (GSTD_TYPE_PIPELINE_BUS,  NULL));
}

static void
gstd_pipeline_bus_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  /*  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);*/

  switch (property_id) {
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}
