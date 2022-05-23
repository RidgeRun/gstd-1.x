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

/* Code based on testd.c found in 
 * http://0pointer.de/lennart/projects/libdaemon/reference/html/testd_8c-example.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_daemon.h"
#include "gstd_log.h"

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <glib/gstdio.h>
#include <libdaemon/dlog.h>
#include <libdaemon/dfork.h>
#include <libdaemon/dsignal.h>
#include <libdaemon/dpid.h>
#include <libdaemon/dexec.h>

static gboolean gstd_daemon_start_parent (void);
static gboolean gstd_daemon_start_child (void);
static const gchar *gstd_daemon_pid (void);
static gchar *gstd_daemon_get_pid_filename (const gchar * filename);

static gboolean _initialized = FALSE;
static gchar *pid_filename = NULL;

gboolean
gstd_daemon_init (gint argc, gchar * argv[], gchar * pidfilename)
{
  const gchar *process_name;
  gchar *pid_path = NULL;
  gboolean ret = TRUE;

  g_return_val_if_fail (argv, FALSE);

  if (_initialized)
    goto out;

  /* Check if pid dir available */
  pid_path = gstd_daemon_get_pid_filename (pidfilename);

  if (NULL == pid_path) {
    g_printerr ("Unable to access Gstd pid dir: pid path is NULL\n");
    ret = FALSE;
    goto out;
  }

  if (g_access (pid_path, W_OK)) {
    g_printerr ("Unable to open Gstd pid dir %s: %s\n", pid_path, g_strerror (errno));
    ret = FALSE;
    goto free_path;
  }

  /* Sanitize the process name to use it as PID identification */
  process_name = daemon_ident_from_argv0 (argv[0]);

  /* Use the process name as the identification prefix for the 
     pid file */
  daemon_pid_file_ident = process_name;

  /*
     Create a gstd.pid file in another path, it could be 
     nullable  */
  pid_filename = pidfilename;

  /* Override the default pid file location to /tmp/ to avoid the need
     of root privileges */
  daemon_pid_file_proc = gstd_daemon_pid;

  _initialized = TRUE;

free_path:
    g_free (pid_path);
out:
    return ret;
}

gboolean
gstd_daemon_start (gboolean * parent)
{
  pid_t pid;
  gboolean ret = FALSE;

  g_return_val_if_fail (_initialized, FALSE);
  g_return_val_if_fail (parent, FALSE);

  /* Check that the daemon is not rung twice a the same time */
  pid = daemon_pid_file_is_running ();
  if (pid >= 0) {
    GST_ERROR ("Daemon already running on PID file %u", pid);
    goto out;
  }

  /* Prepare for return value passing from the initialization
     procedure of the daemon process */
  if (daemon_retval_init () < 0) {
    GST_ERROR ("Failed to create pipe.");
    goto out;
  }

  pid = daemon_fork ();

  /* Negative PID means fork failure */
  if (pid < 0) {
    daemon_retval_done ();
    goto out;
  }
  /* Positive PID is for parent flow */
  if (pid > 0) {
    ret = gstd_daemon_start_parent ();
    *parent = TRUE;
  } else {
    ret = gstd_daemon_start_child ();
    *parent = FALSE;
  }

out:
  {
    return ret;
  }
}

static gboolean
gstd_daemon_start_parent (void)
{
  gint iret;
  gboolean ret = FALSE;
  guint timeout = 20;

  g_return_val_if_fail (_initialized, FALSE);

  /* Wait for 20 seconds for the return value passed from the daemon
     process */
  iret = daemon_retval_wait (timeout);
  if (iret < 0) {
    GST_ERROR ("Could not recieve return value from daemon process: %s",
        g_strerror (errno));
  } else if (iret > 0) {
    GST_ERROR ("Child process ended unexpectedly with code: %d", ret);
  } else {
    ret = TRUE;
  }

  return ret;
}

static gboolean
gstd_daemon_start_child (void)
{
  gint retval = 0;
  gboolean ret;

  GST_INFO ("Detached form parent process");

  /* Create the PID file */
  if (daemon_pid_file_create () < 0) {
    GST_ERROR ("Could not create PID file: %s (%s).", gstd_daemon_pid (),
        g_strerror (errno));
    goto error;
  }

  GST_INFO ("Daemon successfully started");
  retval = 0;
  ret = TRUE;

  goto out;

error:
  {
    ret = FALSE;
    retval = 1;
  }
out:
  {
    daemon_retval_send (retval);
    return ret;
  }
}

gboolean
gstd_daemon_stop (void)
{
  gboolean ret = FALSE;
  guint timeout = 5;

  ret = daemon_pid_file_kill_wait (SIGTERM, timeout);
  if (ret < 0) {
    GST_ERROR
        ("No running Gstd found or gstd.pid was saved using \"--pid-path\"");
    ret = FALSE;
  } else {
    ret = TRUE;
  }

  /* Cleanup the PID file */
  daemon_pid_file_remove ();

  return ret;
}

static const gchar *
gstd_daemon_pid (void)
{
  static gchar *fn = NULL;
  gchar *filename;

  if (fn) {
    g_free (fn);
  }

  filename = gstd_daemon_get_pid_filename (pid_filename);
  fn = g_strdup_printf ("%s/%s.pid", filename,
      daemon_pid_file_ident ? daemon_pid_file_ident : "gstd");
  g_free (filename);

  return fn;

}

static gchar *
gstd_daemon_get_pid_filename (const gchar * filename)
{
  if (filename == NULL)
    return g_strdup (GSTD_RUN_STATE_DIR);

  if (g_path_is_absolute (filename)) {
    return g_strdup (filename);
  } else {
    g_printerr ("The pid filename is not absolute since default filename\n");
    return g_strdup (GSTD_RUN_STATE_DIR);
  }
}
