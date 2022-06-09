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

#ifndef __GSTD_IFORMATTER_H__
#define __GSTD_IFORMATTER_H__

#include <gst/gst.h>
#include "gstd_object.h"

G_BEGIN_DECLS

#define GSTD_TYPE_IFORMATTER                (gstd_iformatter_get_type ())
#define GSTD_IFORMATTER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_IFORMATTER, GstdIFormatter))
#define GSTD_IS_IFORMATTER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_IFORMATTER))
#define GSTD_IFORMATTER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_IFORMATTER, GstdIFormatterInterface))

GType gstd_iformatter_get_type (void);

typedef struct _GstdIFormatter GstdIFormatter;
typedef struct _GstdIFormatterInterface GstdIFormatterInterface;

struct _GstdIFormatterInterface
{
  GTypeInterface parent_iface;

  void (*begin_object) (GstdIFormatter * self);

  void (*end_object) (GstdIFormatter * self);

  void (*begin_array) (GstdIFormatter * self);

  void (*end_array) (GstdIFormatter * self);

  void (*set_member_name) (GstdIFormatter * self, const gchar * name);

  void (*set_string_value) (GstdIFormatter * self, const gchar * value);

  void (*set_value) (GstdIFormatter * self, const GValue * value);

  void (*generate) (GstdIFormatter * self, gchar ** outstring);
};

void gstd_iformatter_begin_object (GstdIFormatter * self);

void gstd_iformatter_end_object (GstdIFormatter * self);

void gstd_iformatter_begin_array (GstdIFormatter * self);

void gstd_iformatter_end_array (GstdIFormatter * self);

void gstd_iformatter_set_member_name (GstdIFormatter * self,
    const gchar * name);

void gstd_iformatter_set_string_value (GstdIFormatter * self,
    const gchar * value);

void gstd_iformatter_set_value (GstdIFormatter * self, const GValue * value);

void gstd_iformatter_generate (GstdIFormatter * self, gchar ** outstring);

G_END_DECLS

#endif /* __GSTD_IFORMATTER_H__ */
