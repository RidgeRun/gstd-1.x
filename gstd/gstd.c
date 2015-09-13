/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
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
#include "gstd.h"

/* GSTD debugging category */
GST_DEBUG_CATEGORY (gstd_debug);

void
gstd_init (gint *argc, gchar **argv[])
{
  guint debug_color;
  GstDebugLevel debug_level;
  
  /* Initialize debug category and setting it to print by default */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  if (!gstd_debug) {
    GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd", debug_color,
			     "GStraemer daemon debug category");

    debug_level = gst_debug_category_get_threshold (gstd_debug);
    if (debug_level < GSTD_DEBUG_DEFAULT_LEVEL)
      debug_level = GSTD_DEBUG_DEFAULT_LEVEL;
    
    gst_debug_set_threshold_for_name ("gstd", debug_level);
    GST_DEBUG ("Initialized gstd debug category");
  }

  gstd_pipeline_init();
}

void
gstd_deinit ()
{
  gstd_pipeline_init();

  if (gstd_debug) {
    GST_DEBUG ("Deinitialized gstd debug category");
    gst_debug_category_free (gstd_debug);
    gstd_debug = NULL;
  }
}

GstdReturnCode
gstd_create_pipeline (gchar *name, gchar *description, gchar **outname)
{
  return gstd_pipeline_create (name, description, outname);
}
