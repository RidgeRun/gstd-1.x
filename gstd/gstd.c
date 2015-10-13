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
#include <unistd.h>
#include <stdlib.h>

#include "gstd.h"

#define GSTD_CLIENT_DEFAULT_PORT 5000

/* GLib main loop, we need it global to access it through the SIGINT
   handler */
static GMainLoop *main_loop;

void
int_handler(gint sig)
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
  GstdSession *session;
  guint port;
  gboolean version;
  GError *error = NULL;
  GOptionContext *context;
  GOptionEntry entries[] = {
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "Attach to the server through the given port (default 5000)",
      "port" },
    { "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Print current gstd version", NULL },
    { NULL }
  };

  // Initialize default
  version = FALSE;
  port = GSTD_CLIENT_DEFAULT_PORT;

  context = g_option_context_new (" - gst-launch under steroids");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gst_init_get_option_group ());
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free(error);
    return EXIT_FAILURE;
  }

  // Print the version and exit
  if (version) {
    g_print ("" PACKAGE_STRING "\n");
    g_print ("Copyright (c) 2015 RigeRun Engineering\n");
    return EXIT_SUCCESS;
  }
  
  /* Initialize GStreamer subsystem before calling anything else */
  gst_init(&argc, &argv);

  /* Install a handler for the interrupt signal */
  signal (SIGINT, int_handler);

  /* Starting the application's main loop, necessary for 
     messaging and signaling subsystem */
  GST_INFO ("Starting application...");
  main_loop = g_main_loop_new (NULL, FALSE);

  session = gstd_new ("Session0", port);
  
  /* Install a handler for the interrupt signal */

  signal (SIGINT, int_handler);
  
  g_main_loop_run (main_loop);

  /* Application shut down*/
  g_main_loop_unref (main_loop);
  main_loop = NULL;

  g_object_unref (session);
  gst_deinit();
  
  return GSTD_EOK;
}
