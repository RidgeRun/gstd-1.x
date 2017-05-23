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

#ifndef __GSTD_MSG_TYPE_H__
#define __GSTD_MSG_TYPE_H__

#include <glib-object.h>

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define GSTD_TYPE_MSG_TYPE (gstd_msg_type_get_type())

GType gstd_msg_type_get_type ();

G_END_DECLS

#endif // __GSTD_MSG_TYPE_H__
