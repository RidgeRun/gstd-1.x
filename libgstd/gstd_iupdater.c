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

#include "gstd_iupdater.h"

G_DEFINE_INTERFACE (GstdIUpdater, gstd_iupdater, G_TYPE_OBJECT);

static void
gstd_iupdater_default_init (GstdIUpdaterInterface * iface)
{

}

GstdReturnCode
gstd_iupdater_update (GstdIUpdater * self, GstdObject * object,
    const gchar * value)
{
  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  return GSTD_IUPDATER_GET_INTERFACE (self)->update (self, object, value);
}
