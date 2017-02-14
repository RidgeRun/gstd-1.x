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

#include "gstd_pipeline_creator.h"
#include "gstd_pipeline.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_creator_debug);
#define GST_CAT_DEFAULT gstd_pipeline_creator_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void gstd_pipeline_creator_create (GstdICreator * iface,
    const gchar * name, const gchar * description, GstdObject ** out);

typedef struct _GstdPipelineCreatorClass GstdPipelineCreatorClass;

/**
 * GstdPipelineCreator:
 * A wrapper for the conventional pipeline_creator
 */
struct _GstdPipelineCreator
{
  GObject parent;
};

struct _GstdPipelineCreatorClass
{
  GObjectClass parent_class;
};


static void
gstd_icreator_interface_init (GstdICreatorInterface * iface)
{
  iface->create = gstd_pipeline_creator_create;
}

G_DEFINE_TYPE_WITH_CODE (GstdPipelineCreator, gstd_pipeline_creator,
    G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE (GSTD_TYPE_ICREATOR,
        gstd_icreator_interface_init));

static void
gstd_pipeline_creator_class_init (GstdPipelineCreatorClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_creator_debug, "gstdpipelinecreator",
      debug_color, "Gstd Pipeline Creator category");
}

static void
gstd_pipeline_creator_init (GstdPipelineCreator * self)
{
  GST_INFO_OBJECT (self, "Initializing pipeline creator");
}

static void
gstd_pipeline_creator_create (GstdICreator * iface, const gchar * name,
    const gchar * description, GstdObject ** out)
{
  GstdPipeline *pipeline;

  g_return_if_fail (iface);
  g_return_if_fail (name);
  g_return_if_fail (description);

  pipeline = g_object_new (GSTD_TYPE_PIPELINE, "name", name, "description",
      description, NULL);

  if (pipeline) {
    *out = GSTD_OBJECT (pipeline);
  } else {
    *out = NULL;
  }
}
