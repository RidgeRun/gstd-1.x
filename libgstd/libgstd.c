/*
 * GStreamer Daemon - gst-launch on steroids
 * C library abstracting gstd
 *
 * Copyright (c) 2015-2021 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define _GNU_SOURCE

#include "libgstd.h"

#include <stdarg.h>
#include <stdio.h>

#include "gstd_http.h"
#include "gstd_ipc.h"
#include "gstd_log.h"
#include "gstd_tcp.h"
#include "gstd_unix.h"
#include "libgstd_assert.h"

static GType gstd_supported_ipc_to_ipc (const SupportedIpcs code);
static void gstd_manager_init (int argc, char *argv[]);

struct _GstDManager
{
  GstdSession *session;
  GstdIpc **ipc_array;
  guint num_ipcs;
};

static GType
gstd_supported_ipc_to_ipc (const SupportedIpcs code)
{
  GType code_description[] = {
    [GSTD_IPC_TYPE_TCP] = GSTD_TYPE_TCP,
    [GSTD_IPC_TYPE_UNIX] = GSTD_TYPE_UNIX,
    [GSTD_IPC_TYPE_HTTP] = GSTD_TYPE_HTTP
  };

  const gint size = sizeof (code_description) / sizeof (gchar *);

  gstd_assert_and_ret_val (0 <= code, GSTD_TYPE_IPC);
  gstd_assert_and_ret_val (size > code, GSTD_TYPE_IPC);

  return code_description[code];
}

static void
gstd_manager_init (int argc, char *argv[])
{
  gst_init (&argc, &argv);
  gstd_debug_init ();
}

GOptionGroup *
gstd_init_get_option_group (void)
{
  return gst_init_get_option_group ();
}

GstdStatus
gstd_manager_new (const SupportedIpcs supported_ipcs[], const guint num_ipcs,
    GstDManager ** out, int argc, char *argv[])
{
  GstdStatus ret = GSTD_LIB_OK;
  GstDManager *manager = NULL;
  GstdSession *session = NULL;
  GstdIpc **ipc_array = NULL;

  gstd_assert_and_ret_val (NULL != out, GSTD_NULL_ARGUMENT);

  manager = (GstDManager *) g_malloc0 (sizeof (*manager));
  session = gstd_session_new ("Session0");

  /* If there is ipcs, then initialize them */
  if (NULL != supported_ipcs && num_ipcs > 0) {
    ipc_array = g_malloc0 (num_ipcs * sizeof (*ipc_array));
    for (guint i = 0; i < num_ipcs; i++) {
      ipc_array[i] =
          GSTD_IPC (g_object_new (gstd_supported_ipc_to_ipc (supported_ipcs[i]),
              NULL));
    }
    manager->ipc_array = ipc_array;
  }

  manager->session = session;
  manager->num_ipcs = num_ipcs;

  *out = manager;

  /* Initialize GStreamer */
  gstd_manager_init (argc, argv);

  return ret;
}

void
gstd_manager_ipc_options (GstDManager * manager, GOptionGroup * ipc_group[])
{
  gint i = 0;

  gstd_assert_and_ret (NULL != manager);
  gstd_assert_and_ret (NULL != manager->ipc_array);
  gstd_assert_and_ret (NULL != ipc_group);

  for (i = 0; i < manager->num_ipcs; i++) {
    gstd_ipc_get_option_group (manager->ipc_array[i], &ipc_group[i]);
  }
}

gboolean
gstd_manager_ipc_start (GstDManager * manager)
{
  gboolean ipc_selected = FALSE;
  gboolean ret = TRUE;
  GstdReturnCode code = GSTD_EOK;
  gint i = 0;

  gstd_assert_and_ret_val (NULL != manager, GSTD_NULL_ARGUMENT);
  gstd_assert_and_ret_val (NULL != manager->ipc_array, GSTD_LIB_NOT_FOUND);
  gstd_assert_and_ret_val (NULL != manager->session, GSTD_LIB_NOT_FOUND);

  /* Verify if at least one IPC mechanism was selected */
  for (i = 0; i < manager->num_ipcs; i++) {
    g_object_get (G_OBJECT (manager->ipc_array[i]), "enabled", &ipc_selected,
        NULL);

    if (ipc_selected) {
      break;
    }
  }

  /* If no IPC was selected, default to TCP */
  if (!ipc_selected) {
    g_object_set (G_OBJECT (manager->ipc_array[0]), "enabled", TRUE, NULL);
  }

  /* Run start for each IPC (each start method checks for the enabled flag) */
  for (i = 0; i < manager->num_ipcs; i++) {
    code = gstd_ipc_start (manager->ipc_array[i], manager->session);
    if (code) {
      g_printerr ("Couldn't start IPC : (%s)\n",
          G_OBJECT_TYPE_NAME (manager->ipc_array[i]));
      ret = FALSE;
    }
  }

  return ret;
}

void
gstd_manager_ipc_stop (GstDManager * manager)
{
  gint i = 0;

  gstd_assert_and_ret (NULL != manager);
  gstd_assert_and_ret (NULL != manager->ipc_array);
  gstd_assert_and_ret (NULL != manager->session);

  /* Run stop for each IPC */
  for (i = 0; i < manager->num_ipcs; i++) {
    if (NULL != manager->ipc_array[i] && TRUE == manager->ipc_array[i]->enabled) {
      gstd_ipc_stop (manager->ipc_array[i]);
      g_clear_object (&manager->ipc_array[i]);
    }
  }
}

void
gstd_manager_free (GstDManager * manager)
{
  gstd_assert_and_ret (NULL != manager);
  gstd_manager_ipc_stop (manager);
  g_free (manager->ipc_array);
  g_object_unref (manager->session);
  g_free (manager);
  gst_deinit ();
}
