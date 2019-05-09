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
#ifndef __GSTD_UNIX_H__
#define __GSTD_UNIX_H__

#include <gio/gio.h>
#include "gstd_return_codes.h"
#include "gstd_session.h"
#include "gstd_ipc.h"
#include "gstd_socket.h"

G_BEGIN_DECLS
#define GSTD_UNIX_DEFAULT_BASE_NAME  "gstd_default_unix_socket"
#define GSTD_UNIX_DEFAULT_NUM_PORTS  1

#define GSTD_TYPE_UNIX \
  (gstd_unix_get_type())
#define GSTD_UNIX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_UNIX,GstdUnix))
#define GSTD_UNIX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_UNIX,GstdUnixClass))
#define GSTD_IS_UNIX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_UNIX))
#define GSTD_IS_UNIX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_UNIX))
#define GSTD_UNIX_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_TPC, GstdUnixClass))
typedef struct _GstdUnix GstdUnix;
typedef struct _GstdUnixClass GstdUnixClass;
GType gstd_unix_get_type ();


G_END_DECLS
#endif //__GSTD_UNIX_H__
