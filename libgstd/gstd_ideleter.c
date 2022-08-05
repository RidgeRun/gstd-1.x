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

#include "gstd_ideleter.h"

G_DEFINE_INTERFACE (GstdIDeleter, gstd_ideleter, G_TYPE_OBJECT);

static void
gstd_ideleter_default_init (GstdIDeleterInterface * iface)
{

}

GstdReturnCode
gstd_ideleter_delete (GstdIDeleter * self, GstdObject * object)
{
  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);

  return GSTD_IDELETER_GET_INTERFACE (self)->delete (self, object);

}
