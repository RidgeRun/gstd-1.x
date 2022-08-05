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

#ifndef __GSTD_LOG_H__
#define __GSTD_LOG_H__

#include <gst/gst.h>

gboolean gstd_log_init (const gchar * gstdfilename, const gchar * gstfilename);
void gstd_log_deinit (void);
void gstd_debug_init (void);

gchar *gstd_log_get_current_gstd (void);
gchar *gstd_log_get_current_gst (void);

/* A default category for every object not defining its own */
GST_DEBUG_CATEGORY_EXTERN (gstd_debug);
#define GST_CAT_DEFAULT gstd_debug

#endif // __GSTD_LOG_H__
