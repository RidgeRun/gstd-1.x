/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
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

G_END_DECLS
#endif // __GSTD_PIPELINE_H__
