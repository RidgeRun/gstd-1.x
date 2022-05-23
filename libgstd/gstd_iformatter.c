/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
