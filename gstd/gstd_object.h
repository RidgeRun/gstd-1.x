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
#ifndef __GSTD_OBJECT_H__
#define __GSTD_OBJECT_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include "gstd_return_codes.h"

G_BEGIN_DECLS

#define GSTD_TYPE_OBJECT \
  (gstd_object_get_type())
#define GSTD_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_OBJECT,GstdObject))
#define GSTD_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_OBJECT,GstdObjectClass))
#define GSTD_IS_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_OBJECT))
#define GSTD_IS_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_OBJECT))

typedef struct _GstdObject GstdObject;
typedef struct _GstdObjectClass GstdObjectClass;
     
struct _GstdObject
{
  GObject parent;

  /**
   * The name of the core session
   */
  gchar *name;
};

struct _GstdObjectClass
{
  GObjectClass parent_class;
};

GType gstd_object_get_type(void);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GstdObject, g_object_unref)

#define GSTD_OBJECT_DEFAULT_NAME "GstdCore0"

G_END_DECLS

#endif //__GSTD_OBJECT_H__
