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

#include "gstd_ireader.h"

G_DEFINE_INTERFACE (GstdIReader, gstd_ireader, G_TYPE_OBJECT);

static void
gstd_ireader_default_init (GstdIReaderInterface * iface)
{

}

GstdReturnCode
gstd_ireader_read (GstdIReader * self, GstdObject * object, const gchar * name,
    GstdObject ** out)
{
  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  return GSTD_IREADER_GET_INTERFACE (self)->read (self, object, name, out);
}
