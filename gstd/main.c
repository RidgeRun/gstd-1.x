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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <signal.h>

#include "return_codes.h"

/* GSTD debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd);
#define GST_CAT_DEFAULT gstd

/* GLib main loop, we need it global to access it through the SIGINT
   handler */
GMainLoop *main_loop;

void
int_handler(int sig)
{
  g_return_if_fail (main_loop);

  /* User has pressed CTRL-C, stop the main loop
     so the application closes itself */
  GST_INFO ("Interrupt received, shutting down...");
  g_main_loop_quit (main_loop);
}

gint
main (gint argc, gchar *argv[])
{
  guint debug_color = 0;
  
  /* Initialize GStreamer subsystem before calling anything else */
  gst_init(&argc, &argv);

  /* Install a handler for the interrupt signal */
  signal (SIGINT, int_handler);

  /* Initialize debug category and setting it to print by default */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd, "gstd", debug_color,
			   "GStraemer daemon debug category");

  gst_debug_set_threshold_for_name ("gstd", GST_LEVEL_INFO);
  
  /* Starting the application's main loop, necessary for 
     messaging and signaling subsystem */
  GST_INFO ("Starting application...");
  main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (main_loop);

  /* Application shut down*/
  g_main_loop_unref (main_loop);
  main_loop = NULL;

  return GSTD_EOK;
}
