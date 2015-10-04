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

#ifndef __GSTD_LIST_H__
#define __GSTD_LIST_H__

#include <glib.h>
#include "gstd_object.h"

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define GSTD_TYPE_LIST gstd_list_get_type ()
G_DECLARE_FINAL_TYPE (GstdList, gstd_list, GSTD, LIST, GstdObject);

G_END_DECLS

#endif // __GSTD_LIST_H__
