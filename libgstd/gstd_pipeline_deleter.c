/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
