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

#ifndef __GSTD_IREADER_H__
#define __GSTD_IREADER_H__

#include <gst/gst.h>
#include "gstd_return_codes.h"

G_BEGIN_DECLS
#define GSTD_TYPE_IREADER                (gstd_ireader_get_type ())
#define GSTD_IREADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_IREADER, GstdIReader))
#define GSTD_IS_IREADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_IREADER))
#define GSTD_IREADER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_IREADER, GstdIReaderInterface))
typedef struct _GstdIReader GstdIReader;
typedef struct _GstdIReaderInterface GstdIReaderInterface;

// Avoid cyclic dependecies by forward declaration
typedef struct _GstdObject GstdObject;

struct _GstdIReaderInterface
{
  GTypeInterface parent;

  GstdReturnCode (*read) (GstdIReader * self, GstdObject * object,
      const gchar * name, GstdObject ** out);
};

GType gstd_ireader_get_type (void);

GstdReturnCode gstd_ireader_read (GstdIReader * self, GstdObject * object,
    const gchar * name, GstdObject ** out);

G_END_DECLS
#endif // __GSTD_IREADER_H__
