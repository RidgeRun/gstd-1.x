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

  void (*begin_object) (GstdIFormatter *self);

  void (*end_object) (GstdIFormatter *self);

  void (*begin_array) (GstdIFormatter *self);

  void (*end_array) (GstdIFormatter *self);

  void (*set_member_name) (GstdIFormatter *self, const gchar * name);

  void (*set_string_value) (GstdIFormatter *self, const gchar * value);

  void (*set_value) (GstdIFormatter *self, GValue *value);

  void (*generate) (GstdIFormatter *self, gchar **outstring);
};

void gstd_iformatter_begin_object (GstdIFormatter *self);

void gstd_iformatter_end_object (GstdIFormatter *self);

void gstd_iformatter_begin_array (GstdIFormatter *self);

void gstd_iformatter_end_array (GstdIFormatter *self);

void gstd_iformatter_set_member_name (GstdIFormatter *self, const gchar * name);

void gstd_iformatter_set_string_value (GstdIFormatter *self, const gchar * value);

void gstd_iformatter_set_value (GstdIFormatter *self, GValue *value);

void gstd_iformatter_generate (GstdIFormatter *self, gchar **outstring);

G_END_DECLS


#endif /* __GSTD_IFORMATTER_H__ */
