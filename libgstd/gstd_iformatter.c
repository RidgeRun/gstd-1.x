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
gstd_iformatter_set_string_value (GstdIFormatter * self, const gchar * value)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->set_string_value (self, value);
}

void
gstd_iformatter_set_null_value (GstdIFormatter * self)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->set_null_value (self);
}

void
gstd_iformatter_set_value (GstdIFormatter * self, const GValue * value)
{
  g_return_if_fail (self);
  GSTD_IFORMATTER_GET_INTERFACE (self)->set_value (self, value);
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
