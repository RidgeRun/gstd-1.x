/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015-2017 RidgeRun Engineering <support@ridgerun.com>
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

#include "gstd_iformatter.h"

G_DEFINE_INTERFACE (GstdIFormatter, gstd_iformatter, G_TYPE_OBJECT);


void
gstd_iformatter_begin_object (GstdIFormatter * self)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->begin_object (self);
}

void
gstd_iformatter_end_object (GstdIFormatter * self)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->end_object (self);
}

void
gstd_iformatter_begin_array (GstdIFormatter * self)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->begin_array (self);
}

void
gstd_iformatter_end_array (GstdIFormatter * self)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->end_array (self);
}

void
gstd_iformatter_set_member_name (GstdIFormatter * self, const gchar * name)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->set_member_name (self, name);
}

void
gstd_iformatter_set_member_value (GstdIFormatter * self, const gchar * value)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->set_member_value (self, value);
}

void
gstd_iformatter_generate (GstdIFormatter * self, gchar ** outstring)
{
  g_return_if_fail (self);

  GSTD_IFORMATTER_GET_INTERFACE (self)->generate (self, outstring);
}

static void
gstd_iformatter_default_init (GstdIFormatterInterface * iface)
{
  /* Add properties and signals to the interface here */
}
