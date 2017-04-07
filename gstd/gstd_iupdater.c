/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#include "gstd_iupdater.h"

G_DEFINE_INTERFACE (GstdIUpdater, gstd_iupdater, G_TYPE_OBJECT);

static void
gstd_iupdater_default_init (GstdIUpdaterInterface * iface)
{

}

GstdReturnCode
gstd_iupdater_update (GstdIUpdater * self, GstdObject * object, const gchar * value)
{
  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  return GSTD_IUPDATER_GET_INTERFACE (self)->update (self, object, value);
}
