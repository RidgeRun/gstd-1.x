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

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <gst/gst.h>

#include "gstd_session.h"
#include "gstd_ipc.h"
#include "gstd_tcp.h"

#define GSTD_CLIENT_DEFAULT_PORT 5000

/* GLib main loop, we need it global to access it through the SIGINT
   handler */
static GMainLoop *main_loop;

void
int_handler (gint sig)
{
  g_return_if_fail (main_loop);

  /* User has pressed CTRL-C, stop the main loop
     so the application closes itself */
  GST_INFO ("Interrupt received, shutting down...");
  g_main_loop_quit (main_loop);
}

gint
main (gint argc, gchar * argv[])
{
  GstdSession *session;
  guint i;
  gboolean version;
  GError *error = NULL;
  GOptionContext *context;
  GOptionGroup *gstreamer_group;
  gboolean ipc_selected = FALSE;
  GstdReturnCode ret;
  /* Array to specify gstd how many IPCs are supported, 
   * IPCs should be added this array.
   */
  GType supported_ipcs[] = {
    GSTD_TYPE_TCP,
  };

  guint num_ipcs = (sizeof (supported_ipcs) / sizeof (GType));
  GstdIpc *ipc_array[num_ipcs];
  GOptionGroup *optiongroup_array[num_ipcs];

  GOptionEntry entries[] = {
    {"version", 'v', 0, G_OPTION_ARG_NONE, &version,
        "Print current gstd version", NULL}
    ,
    {NULL}
  };

  /* Initialize default */
  version = FALSE;
  context = g_option_context_new (" - gst-launch under steroids");
  g_option_context_add_main_entries (context, entries, NULL);

  gstreamer_group = gst_init_get_option_group ();
  g_option_context_add_group (context, gstreamer_group);

  /* Initialize GStreamer subsystem before calling anything else */
  gst_init (&argc, &argv);

  /* Install a handler for the interrupt signal */
  signal (SIGINT, int_handler);

  /* Starting the application's main loop, necessary for 
     messaging and signaling subsystem */
  GST_INFO ("Starting application...");
  main_loop = g_main_loop_new (NULL, FALSE);

  /*Create session */
  session = gstd_session_new ("Session0");

  /* Read option group for each IPC */
  for (i = 0; i < num_ipcs; i++) {
    ipc_array[i] = GSTD_IPC (g_object_new (supported_ipcs[i], NULL));
    gstd_ipc_get_option_group (ipc_array[i], &optiongroup_array[i]);
    g_option_context_add_group (context, optiongroup_array[i]);
  }

  /*Parse the options before starting any IPC */
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return EXIT_FAILURE;
  }

  /* Print the version and exit */
  if (version) {
    g_print ("" PACKAGE_STRING "\n");
    g_print ("Copyright (c) 2015 RigeRun Engineering\n");
    return EXIT_SUCCESS;
  }

  /* If no IPC selected use tcp */
  for (i = 0; i < num_ipcs; i++) {
    g_object_get (G_OBJECT(ipc_array[i]), "enabled", &ipc_selected, NULL);

    if (ipc_selected) {
      break;
    }
  }

  if (!ipc_selected) {
    g_object_set(G_OBJECT(ipc_array[0]), "enabled", TRUE, NULL);
  }

  /* Run start for each IPC (each start method checks for the enabled flag) */
  for (i = 0; i < num_ipcs; i++) {
    ret = gstd_ipc_start (ipc_array[i], session);
    if(ret)
      {
	g_printerr ("Couldn't start IPC : (%s)\n", G_OBJECT_TYPE_NAME(ipc_array[i]));
	return EXIT_FAILURE;
      }
  }

  /* Install a handler for the interrupt signal */

  signal (SIGINT, int_handler);

  g_main_loop_run (main_loop);
  /* Application shut down */
  g_main_loop_unref (main_loop);
  main_loop = NULL;

  /* Run stop for each IPC */
  for (i = 0; i < num_ipcs; i++) {
    gstd_ipc_stop (ipc_array[i]);
    g_object_unref (ipc_array[i]);
  }
  g_object_unref (session);
  gst_deinit ();

  return GSTD_EOK;
}
