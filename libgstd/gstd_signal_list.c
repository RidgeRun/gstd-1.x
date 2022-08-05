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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "gstd_signal.h"
#include "gstd_signal_list.h"


/* Gstd List debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_signal_list_debug);
#define GST_CAT_DEFAULT gstd_signal_list_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


G_DEFINE_TYPE (GstdSignalList, gstd_signal_list, GSTD_TYPE_LIST);

/* VTable */
static void gstd_signal_list_dispose (GObject *);

static void
gstd_signal_list_class_init (GstdSignalListClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  guint debug_color;

  object_class->dispose = gstd_signal_list_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_signal_list_debug, "gstdsignallist",
      debug_color, "Gstd Signal List category");
}

static void
gstd_signal_list_init (GstdSignalList * self)
{
  GST_INFO_OBJECT (self, "Initializing signal list");
}

static void
gstd_signal_list_dispose (GObject * object)
{
  GstdSignalList *self = GSTD_SIGNAL_LIST (object);
  GstdList *list = GSTD_LIST (self);

  GST_INFO_OBJECT (self, "Disposing %s signal list", GSTD_OBJECT_NAME (self));

  if (list->list) {
    GList *elem;
    for (elem = list->list; elem; elem = g_list_next (elem)) {
      gstd_signal_disconnect (elem->data);
    }

    g_list_free_full (list->list, g_object_unref);
    list->list = NULL;
  }

  G_OBJECT_CLASS (gstd_signal_list_parent_class)->dispose (object);
}
