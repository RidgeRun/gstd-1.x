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
#ifndef __GSTD_DEBUG_H__
#define __GSTD_DEBUG_H__

#include <glib.h>
#include <gst/gst.h>

//GST_DEBUG_CATEGORY_EXTERN (gstd_debug);
//#define GST_CAT_DEFAULT gstd_debug

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
GType gstd_debug_get_type();

/**
 * gstd_debug_new: (constructor)
 * @lvl: (non nullable): Debug level.
 * 
 * Creates a new object to handle debuggin state.
 *
 * Returns: (transfer full) (nullable): A new #GstdDebug. Free after
 * usage using g_object_unref()
 */
GstdDebug *
gstd_debug_new (GstDebugLevel lvl);


#endif // __GSTD_DEBUG_H__

