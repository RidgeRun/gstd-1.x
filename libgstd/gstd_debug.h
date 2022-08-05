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
#ifndef __GSTD_DEBUG_H__
#define __GSTD_DEBUG_H__

#include <gst/gst.h>

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_BEGIN_DECLS
#define GSTD_TYPE_DEBUG \
  (gstd_debug_get_type())
#define GSTD_DEBUG(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_DEBUG,GstdDebug))
#define GSTD_DEBUG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_DEBUG,GstdDebugClass))
#define GSTD_IS_DEBUG(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_DEBUG))
#define GSTD_IS_DEBUG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_DEBUG))
#define GSTD_DEBUG_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_DEBUG, GstdDebugClass))
typedef struct _GstdDebug GstdDebug;
typedef struct _GstdDebugClass GstdDebugClass;
GType gstd_debug_get_type (void);

/**
 * gstd_debug_new: (constructor)
 * 
 * Creates a new object to handle debugging options.
 *
 * Returns: (transfer full) (nullable): A new #GstdDebug. Free after
 * usage using g_object_unref()
 */
GstdDebug *gstd_debug_new (void);


#endif // __GSTD_DEBUG_H__
