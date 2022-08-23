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

#ifndef __GSTD_PIPELINE_BUS_H__
#define __GSTD_PIPELINE_BUS_H__

#include <gst/gst.h>
#include <gstd_object.h>

G_BEGIN_DECLS
#define GSTD_TYPE_PIPELINE_BUS \
  (gstd_pipeline_bus_get_type())
#define GSTD_PIPELINE_BUS(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_PIPELINE_BUS,GstdPipelineBus))
#define GSTD_PIPELINE_BUS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_PIPELINE_BUS,GstdPipelineBusClass))
#define GSTD_IS_PIPELINE_BUS(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_PIPELINE_BUS))
#define GSTD_IS_PIPELINE_BUS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_PIPELINE_BUS))
#define GSTD_PIPELINE_BUS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_PIPELINE_BUS, GstdPipelineBusClass))

typedef struct _GstdPipelineBus GstdPipelineBus;
typedef struct _GstdPipelineBusClass GstdPipelineBusClass;

GType gstd_pipeline_bus_get_type (void);

/**
 * gstd_pipeline_bus_new: (constructor)
 *
 * Creates a new object that captures data from the pipeline bus.
 *
 * Returns: (transfer full) (nullable): A new #GstdPipelinebus. Free after
 * usage using g_object_unref()
 */
GstdPipelineBus *gstd_pipeline_bus_new (GstBus * bus);


GstBus *gstd_pipeline_bus_get_bus (GstdPipelineBus * self);


G_END_DECLS

#endif // __GSTD_PIPELINE_BUS_H__
