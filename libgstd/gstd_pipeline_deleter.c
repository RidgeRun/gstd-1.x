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

#include "gstd_pipeline_deleter.h"
#include "gstd_pipeline.h"


#include <gobject/gvaluecollector.h>
#include <gst/gst.h>
#include <string.h>

#include "gstd_list.h"
#include "gstd_object.h"


/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_deleter_debug);
#define GST_CAT_DEFAULT gstd_pipeline_deleter_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode gstd_pipeline_deleter_delete (GstdIDeleter * iface,
    GstdObject * object);

typedef struct _GstdPipelineDeleterClass GstdPipelineDeleterClass;

/**
 * GstdPipelineDeleter:
 * A wrapper for the conventional pipeline_deleter
 */
struct _GstdPipelineDeleter
{
  GObject parent;
};

struct _GstdPipelineDeleterClass
{
  GObjectClass parent_class;
};


static void
gstd_ideleter_interface_init (GstdIDeleterInterface * iface)
{
  iface->delete = gstd_pipeline_deleter_delete;
}

G_DEFINE_TYPE_WITH_CODE (GstdPipelineDeleter, gstd_pipeline_deleter,
    G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE (GSTD_TYPE_IDELETER,
        gstd_ideleter_interface_init));

static void
gstd_pipeline_deleter_class_init (GstdPipelineDeleterClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_deleter_debug, "gstdpipelinedeleter",
      debug_color, "Gstd Pipeline Deleter category");
}

static void
gstd_pipeline_deleter_init (GstdPipelineDeleter * self)
{
  GST_INFO_OBJECT (self, "Initializing pipeline deleter");
}

static GstdReturnCode
gstd_pipeline_deleter_delete (GstdIDeleter * iface, GstdObject * object)
{
  GstdObject *state;
  GstdReturnCode ret;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);

  /* Stop the pipe if playing */
  ret = gstd_object_read (object, "state", &state);
  if (ret)
    return ret;

  ret = gstd_object_update (state, "NULL");
  if (ret)
    return ret;

  g_object_unref (state);
  g_object_unref (object);

  return ret;
}
