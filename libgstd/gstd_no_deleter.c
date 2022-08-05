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

#include "gstd_no_deleter.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_no_deleter_debug);
#define GST_CAT_DEFAULT gstd_no_deleter_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode gstd_no_deleter_delete (GstdIDeleter * iface,
    GstdObject * object);

typedef struct _GstdNoDeleterClass GstdNoDeleterClass;

/**
 * GstdNoDeleter:
 * A wrapper for the conventional no_deleter
 */
struct _GstdNoDeleter
{
  GObject parent;
};

struct _GstdNoDeleterClass
{
  GObjectClass parent_class;
};

static void
gstd_ideleter_interface_init (GstdIDeleterInterface * iface)
{
  iface->delete = gstd_no_deleter_delete;
}

G_DEFINE_TYPE_WITH_CODE (GstdNoDeleter, gstd_no_deleter, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (GSTD_TYPE_IDELETER, gstd_ideleter_interface_init));

static void
gstd_no_deleter_class_init (GstdNoDeleterClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_no_deleter_debug, "gstdnodeleter", debug_color,
      "Gstd No Deleter category");
}

static void
gstd_no_deleter_init (GstdNoDeleter * self)
{
  GST_INFO_OBJECT (self, "Initializing no deleter");
}

static GstdReturnCode
gstd_no_deleter_delete (GstdIDeleter * iface, GstdObject * object)
{
  GST_ERROR_OBJECT (iface, "Unable to delete on this resource");

  return GSTD_EOK;
}
