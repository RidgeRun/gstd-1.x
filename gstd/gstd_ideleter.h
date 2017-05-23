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

#ifndef __GSTD_IDELETER_H__
#define __GSTD_IDELETER_H__

#include <gst/gst.h>
#include <gstd_return_codes.h>

G_BEGIN_DECLS
#define GSTD_TYPE_IDELETER                (gstd_ideleter_get_type ())
#define GSTD_IDELETER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_IDELETER, GstdIDeleter))
#define GSTD_IS_IDELETER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_IDELETER))
#define GSTD_IDELETER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_IDELETER, GstdIDeleterInterface))
typedef struct _GstdIDeleter GstdIDeleter;
typedef struct _GstdIDeleterInterface GstdIDeleterInterface;

// Avoid cyclic dependecies by forward declaration
typedef struct _GstdObject GstdObject;

struct _GstdIDeleterInterface
{
  GTypeInterface parent;

  GstdReturnCode (*delete) (GstdIDeleter * self, GstdObject * object);
};

GType gstd_ideleter_get_type (void);

GstdReturnCode gstd_ideleter_delete (GstdIDeleter * self, GstdObject * object);

G_END_DECLS
#endif // __GSTD_IDELETER_H__
