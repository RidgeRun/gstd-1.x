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

#include "gstd_ideleter.h"

G_DEFINE_INTERFACE (GstdIDeleter, gstd_ideleter, G_TYPE_OBJECT);

static void
gstd_ideleter_default_init (GstdIDeleterInterface *iface)
{

}

void
gstd_ideleter_delete (GstdIDeleter *self, GstdObject *object)
{
  g_return_if_fail (self);

  GSTD_IDELETER_GET_INTERFACE (self)->delete (self, object);
}
