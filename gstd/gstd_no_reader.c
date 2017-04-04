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

#include "gstd_no_reader.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_no_reader_debug);
#define GST_CAT_DEFAULT gstd_no_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdObject * gstd_no_reader_read (GstdIReader * iface, GstdObject * object, const gchar * name);

typedef struct _GstdNoReaderClass GstdNoReaderClass;

/**
 * GstdNoReader:
 * A wrapper for the conventional no_reader
 */
struct _GstdNoReader
{
  GObject parent;
};

struct _GstdNoReaderClass
{
  GObjectClass parent_class;
};

static void
gstd_ireader_interface_init (GstdIReaderInterface * iface)
{
  iface->read = gstd_no_reader_read;
}

G_DEFINE_TYPE_WITH_CODE (GstdNoReader, gstd_no_reader, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (GSTD_TYPE_IREADER, gstd_ireader_interface_init));

static void
gstd_no_reader_class_init (GstdNoReaderClass * klass)
{
  guint debug_color;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_no_reader_debug, "gstdnoreader", debug_color,
      "Gstd No Reader category");
}

static void
gstd_no_reader_init (GstdNoReader * self)
{
  GST_INFO_OBJECT (self, "Initializing no reader");
}

static GstdObject *
gstd_no_reader_read (GstdIReader * iface, GstdObject * object, const gchar * name)
{
  GST_ERROR_OBJECT (iface, "Unable to read from this resource");
  return NULL;
}
