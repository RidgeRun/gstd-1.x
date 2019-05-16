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
#ifndef __GSTD_SOCKET_H__
#define __GSTD_SOCKET_H__

#include <gio/gio.h>

#include "gstd_ipc.h"

G_BEGIN_DECLS
#define GSTD_TYPE_SOCKET \
  (gstd_socket_get_type())
#define GSTD_SOCKET(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_SOCKET,GstdSocket))
#define GSTD_SOCKET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_SOCKET,GstdSocketClass))
#define GSTD_IS_SOCKET(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_SOCKET))
#define GSTD_IS_SOCKET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_SOCKET))
#define GSTD_SOCKET_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_SOCKET, GstdSocketClass))
typedef struct _GstdSocket GstdSocket;
typedef struct _GstdSocketClass GstdSocketClass;

struct _GstdSocket
{
  GstdIpc parent;
  GSocketService *service;
};

struct _GstdSocketClass
{
  GstdIpcClass parent_class;

  GstdReturnCode (*add_listener_address) (GstdSocket *, GSocketService **);
};

GType gstd_socket_get_type ();

G_END_DECLS
#endif //__GSTD_SOCKET_H__
