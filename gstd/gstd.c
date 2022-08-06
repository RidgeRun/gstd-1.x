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

#include <stdlib.h>

#include "gstd.h"
#include "gstd_daemon.h"
#include "gstd_log.h"

#define HEADER \
      "\nGstD version " PACKAGE_VERSION "\n" \
      "Copyright (C) 2015-2021 RidgeRun (https://www.ridgerun.com)\n\n"

static gboolean int_term_handler (gpointer user_data);
static void print_header ();

static void
print_header (void)
{
  g_print (HEADER);
}

static gboolean
int_term_handler (gpointer user_data)
{
  GMainLoop *main_loop;

  g_return_val_if_fail (user_data, TRUE);

  main_loop = (GMainLoop *) user_data;

  /* User has pressed CTRL-C, stop the main loop
     so the application closes itself */
  GST_INFO ("Interrupt received, shutting down...");
  g_print ("\n");
  g_main_loop_quit (main_loop);

  return TRUE;
}

gint
main (gint argc, gchar * argv[])
{
  GMainLoop *main_loop;
  gboolean version = FALSE;
  gboolean kill = FALSE;
  gboolean daemon = FALSE;
  gboolean quiet = FALSE;
  const gchar *gstdlogfile = NULL;
  const gchar *gstlogfile = NULL;
  gchar *pidfile = NULL;
  GError *error = NULL;
  GOptionContext *context = NULL;
  gint ret = EXIT_SUCCESS;
  gchar *current_filename = NULL;
  gchar *filename = NULL;
  gboolean nolog = FALSE;
  gboolean parent = FALSE;

  GstD *gstd = NULL;

  GOptionEntry entries[] = {
    {"version", 'v', 0, G_OPTION_ARG_NONE, &version,
        "Print current gstd version and exit", NULL}
    ,
    {"kill", 'k', 0, G_OPTION_ARG_NONE, &kill,
        "Kill a running gstd, if any", NULL}
    ,
    {"quiet", 'q', 0, G_OPTION_ARG_NONE, &quiet,
        "Don't print any startup message", NULL}
    ,
    {"daemon", 'e', 0, G_OPTION_ARG_NONE, &daemon,
        "Detach into a daemon", NULL}
    ,
    {"pid-path", 'f', 0, G_OPTION_ARG_FILENAME, &pidfile,
        "Create gstd.pid file into path", NULL}
    ,
    {"gstd-log-filename", 'l', 0, G_OPTION_ARG_FILENAME, &gstdlogfile,
        "Create gstd.log file to path", NULL}
    ,
    {"gst-log-filename", 'd', 0, G_OPTION_ARG_FILENAME, &gstlogfile,
        "Create gst.log file to path", NULL}
    ,
    {"no-log", 'L', 0, G_OPTION_ARG_NONE, &nolog,
          "Disable file logging when gstd is running in daemon mode. Takes precedence over -l and -d.",
        NULL}
    ,
    {NULL}
  };

  /* Initialize default */
  context = g_option_context_new (" - gst-launch under steroids");
  g_option_context_add_main_entries (context, entries, NULL);

  /* Initialize GStreamer */
  gstd_new (&gstd, 0, NULL);
  gstd_context_add_group (gstd, context);

  /* Parse the options before starting */
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return EXIT_FAILURE;
  }
  g_option_context_free (context);

  if (!quiet && !kill) {
    print_header ();
  }

  if (version) {
    goto out;
  }

  /* If we need to daemonize or interact with the daemon (like killing
   * it, for example) we need to initialize the daemon subsystem.
   */
  if (daemon || kill) {

    /* Initialize the file logging only if:
     * - the user didn't explicitly request it by setting --no-log
     * - the user didn't invoke gstd to kill the daemon
     */
    if (!nolog && !kill) {
      if (!gstd_log_init (gstdlogfile, gstlogfile)) {
        ret = EXIT_FAILURE;
        goto out;
      }
    }

    if (!gstd_daemon_init (argc, argv, pidfile)) {
      ret = EXIT_FAILURE;
      goto out;
    }
  }

  gstd_debug_init ();

  if (kill) {
    if (gstd_daemon_stop ()) {
      GST_INFO ("Gstd successfully stopped");
    }
    goto out;
  }

  if (daemon) {
    if (!gstd_daemon_start (&parent)) {
      goto error;
    }

    /* Parent fork ends here */
    if (parent) {
      if (!quiet) {
        filename = gstd_log_get_current_gstd ();
        if (nolog) {
          g_print ("Log traces have been disabled.\n");
        } else {
          g_print ("Log traces will be saved to %s.\n", filename);
        }
        g_print ("Detaching from parent process.\n");
        g_free (filename);
      }
      goto out;
    }
  }

  /* Start IPC subsystem */
  if (!gstd_start (gstd)) {
    goto error;
  }

  /* Starting the application's main loop, necessary for 
     messaging and signaling subsystem */
  main_loop = g_main_loop_new (NULL, FALSE);

  /* Install a handler for the interrupt signal */
  g_unix_signal_add (SIGINT, int_term_handler, main_loop);

  /* Install a handler for the termination signal */
  g_unix_signal_add (SIGTERM, int_term_handler, main_loop);

  GST_INFO ("Gstd started");
  g_main_loop_run (main_loop);

  /* Application shut down */
  g_main_loop_unref (main_loop);
  main_loop = NULL;

  /* Stop any IPC array */
  gstd_stop (gstd);

  gstd_log_deinit ();

  goto out;

error:
  {
    current_filename = gstd_log_get_current_gstd ();
    GST_ERROR ("Unable to start Gstd. Check %s for more details.",
        current_filename);
    g_free (current_filename);
    ret = EXIT_FAILURE;

  }
out:
  {
    gst_deinit ();
    gstd_free (gstd);
    return ret;
  }
}
