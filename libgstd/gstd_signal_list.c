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
