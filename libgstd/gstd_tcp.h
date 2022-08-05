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
#ifndef __GSTD_TCP_H__
#define __GSTD_TCP_H__

#include <gio/gio.h>
#include "gstd_return_codes.h"
#include "gstd_session.h"
#include "gstd_ipc.h"
#include "gstd_socket.h"

G_BEGIN_DECLS
#define GSTD_TCP_DEFAULT_ADDRESS "127.0.0.1"
#define GSTD_TCP_DEFAULT_PORT 5000
#define GSTD_TCP_DEFAULT_NUM_PORTS 1
#define GSTD_TCP_DEFAULT_MAX_THREADS -1

#define GSTD_TYPE_TCP \
  (gstd_tcp_get_type())
#define GSTD_TCP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_TCP,GstdTcp))
#define GSTD_TCP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_TCP,GstdTcpClass))
#define GSTD_IS_TCP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_TCP))
#define GSTD_IS_TCP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_TCP))
#define GSTD_TCP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_TPC, GstdTcpClass))
typedef struct _GstdTcp GstdTcp;
typedef struct _GstdTcpClass GstdTcpClass;
GType gstd_tcp_get_type (void);


G_END_DECLS
#endif //__GSTD_TCP_H__
