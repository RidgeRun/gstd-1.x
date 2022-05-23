/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gstd_pipeline_creator.h"
#include "gstd_pipeline.h"
#include "gstd_property_reader.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_creator_debug);
#define GST_CAT_DEFAULT gstd_pipeline_creator_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode gstd_pipeline_creator_create (GstdICreator * iface,
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

static GstdReturnCode
gstd_pipeline_creator_create (GstdICreator * iface, const gchar * name,
    const gchar * description, GstdObject ** out)
{
  GstdPipeline *pipeline;
  *out = NULL;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);

  if (NULL == name) {
    GST_ERROR_OBJECT (iface, "Pipeline name not provided");
    return GSTD_MISSING_NAME;
  }

  if (NULL == description) {
    GST_ERROR_OBJECT (iface, "Pipeline description not provided");
    return GSTD_MISSING_ARGUMENT;
  }

  pipeline = g_object_new (GSTD_TYPE_PIPELINE, "name", name, "description",
      description, NULL);
  *out = GSTD_OBJECT (pipeline);

  return gstd_pipeline_build (pipeline);
}
