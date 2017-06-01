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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <gst/gst.h>
#include <glib-unix.h>

#include "gstd_session.h"
#include "gstd_ipc.h"
#include "gstd_tcp.h"

static gboolean int_handler (gpointer user_data);
static void ipc_add_option_groups (GstdIpc * ipc[], GType factory[],
    guint num_ipcs, GOptionContext * context, GOptionGroup * groups[]);
static gboolean ipc_start (GstdIpc * ipc[], guint num_ipcs,
    GstdSession * session);
static void ipc_stop (GstdIpc * ipc[], guint numipc);

static gboolean
int_handler (gpointer user_data)
{
  GMainLoop *main_loop;

  g_return_val_if_fail (user_data, TRUE);

  main_loop = (GMainLoop *) user_data;

  /* User has pressed CTRL-C, stop the main loop
     so the application closes itself */
  GST_INFO ("Interrupt received, shutting down...");
  g_main_loop_quit (main_loop);

  return TRUE;
}

static void
ipc_add_option_groups (GstdIpc * ipc[], GType factory[], guint num_ipcs,
    GOptionContext * context, GOptionGroup * groups[])
{
  gint i;

  g_return_if_fail (ipc);
  g_return_if_fail (context);
  g_return_if_fail (groups);

  for (i = 0; i < num_ipcs; i++) {
    ipc[i] = GSTD_IPC (g_object_new (factory[i], NULL));
    gstd_ipc_get_option_group (ipc[i], &groups[i]);
    g_option_context_add_group (context, groups[i]);
  }
}

static gboolean
ipc_start (GstdIpc * ipc[], guint num_ipcs, GstdSession * session)
{
  gboolean ipc_selected = FALSE;
  gboolean ret = TRUE;
  GstdReturnCode code;
  gint i;

  g_return_val_if_fail (ipc, FALSE);
  g_return_val_if_fail (session, FALSE);

  /* Verify if at leas one IPC mechanism was selected */
  for (i = 0; i < num_ipcs; i++) {
    g_object_get (G_OBJECT (ipc[i]), "enabled", &ipc_selected, NULL);

    if (ipc_selected) {
      break;
    }
  }

  /* If no IPC was selected, default to TCP */
  if (!ipc_selected) {
    g_object_set (G_OBJECT (ipc[0]), "enabled", TRUE, NULL);
  }

  /* Run start for each IPC (each start method checks for the enabled flag) */
  for (i = 0; i < num_ipcs; i++) {
    code = gstd_ipc_start (ipc[i], session);
    if (code) {
      g_printerr ("Couldn't start IPC : (%s)\n", G_OBJECT_TYPE_NAME (ipc[i]));
      ret = FALSE;
    }
  }

  return ret;
}

static void
ipc_stop (GstdIpc * ipc[], guint num_ipcs)
{
  gint i;

  g_return_if_fail (ipc);

  /* Run stop for each IPC */
  for (i = 0; i < num_ipcs; i++) {
    gstd_ipc_stop (ipc[i]);
    g_object_unref (ipc[i]);
  }
}

gint
main (gint argc, gchar * argv[])
{
  GMainLoop *main_loop;
  GstdSession *session;
  gboolean version = FALSE;;
  GError *error = NULL;
  GOptionContext *context;
  GOptionGroup *gstreamer_group;
  gint ret = EXIT_SUCCESS;

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
  context = g_option_context_new (" - gst-launch under steroids");
  g_option_context_add_main_entries (context, entries, NULL);

  /* Initialize GStreamer */
  gstreamer_group = gst_init_get_option_group ();
  g_option_context_add_group (context, gstreamer_group);

  /* Read option group for each IPC */
  ipc_add_option_groups (ipc_array, supported_ipcs, num_ipcs, context,
      optiongroup_array);

  /* Parse the options before starting */
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return EXIT_FAILURE;
  }
  g_option_context_free (context);

  /* Print the version and exit */
  if (version) {
    goto out;
  }

  /*Create session */
  session = gstd_session_new ("Session0");

  /* Start IPC subsystem */
  if (!ipc_start (ipc_array, num_ipcs, session)) {
    goto error;
  }

  /* Starting the application's main loop, necessary for 
     messaging and signaling subsystem */
  main_loop = g_main_loop_new (NULL, FALSE);

  /* Install a handler for the interrupt signal */
  g_unix_signal_add (SIGINT, int_handler, main_loop);

  GST_INFO ("Gstd started");
  g_main_loop_run (main_loop);

  /* Application shut down */
  g_main_loop_unref (main_loop);
  main_loop = NULL;

  /* Stop any IPC array */
  ipc_stop (ipc_array, num_ipcs);

  g_object_unref (session);
  gst_deinit ();

  goto out;

error:
  {
    GST_ERROR ("Unable to start Gstd. Check %s for more details.",
        gstd_log_get_gstd ());
    ret = EXIT_FAILURE;
  }
out:
  {
    return ret;
  }
}
