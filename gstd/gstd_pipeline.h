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

#ifndef __GSTD_PIPELINE_H__
#define __GSTD_PIPELINE_H__

#include <glib-object.h>

#include "gstd_object.h"

G_BEGIN_DECLS
/*
 * Type declaration.
 */
#define GSTD_TYPE_PIPELINE \
  (gstd_pipeline_get_type())
#define GSTD_PIPELINE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_PIPELINE,GstdPipeline))
#define GSTD_PIPELINE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_PIPELINE,GstdPipelineClass))
#define GSTD_IS_PIPELINE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_PIPELINE))
#define GSTD_IS_PIPELINE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_PIPELINE))
#define GSTD_PIPELINE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_PIPELINE, GstdPipelineClass))
typedef struct _GstdPipeline GstdPipeline;
typedef struct _GstdPipelineClass GstdPipelineClass;
GType gstd_pipeline_get_type ();

GstdReturnCode gstd_pipeline_build (GstdPipeline * object);

typedef enum
{
  GSTD_PIPELINE_NULL,
  GSTD_PIPELINE_PAUSED,
  GSTD_PIPELINE_PLAYING,
} GstdPipelineState;

G_END_DECLS
#endif // __GSTD_PIPELINE_H__
