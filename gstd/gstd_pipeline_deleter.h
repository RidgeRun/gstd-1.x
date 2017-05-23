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

#ifndef __GSTD_PIPELINE_DELETER_H__
#define __GSTD_PIPELINE_DELETER_H__

#include <gst/gst.h>

#include "gstd_ideleter.h"

G_BEGIN_DECLS
/*
 * Type declaration.
 */
#define GSTD_TYPE_PIPELINE_DELETER \
  (gstd_pipeline_deleter_get_type())
#define GSTD_PIPELINE_DELETER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_PIPELINE_DELETER,GstdPipelineDeleter))
#define GSTD_PIPELINE_DELETER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_PIPELINE_DELETER,GstdPipelineDeleterClass))
#define GSTD_IS_PIPELINE_DELETER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_PIPELINE_DELETER))
#define GSTD_IS_PIPELINE_DELETER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_PIPELINE_DELETER))
#define GSTD_PIPELINE_DELETER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_PIPELINE_DELETER, GstdPipelineDeleterClass))
typedef struct _GstdPipelineDeleter GstdPipelineDeleter;

GType gstd_pipeline_deleter_get_type ();

G_END_DECLS
#endif // __GSTD_PIPELINE_DELETER_H__
