/*
 * GStreamer Daemon - gst-launch on steroids
 * C client library abstracting gstd interprocess communication
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

#include <glib-unix.h>

#include "libgstd.h"
#include "gstd_ipc.h"
#include "gstd_tcp.h"
#include "gstd_unix.h"
#include "gstd_http.h"
#include "gstd_log.h"
#include "libgstd_assert.h"

#include <stdio.h>

static GType gstd_supported_ipc_to_ipc (Supported_IPCs code);

struct _GstDManager
{
  GstdSession *session;
  GstdIpc **ipc_array;
  guint num_ipcs;
};

static GType
gstd_supported_ipc_to_ipc (Supported_IPCs code)
{
  GType code_description[] = {
    [GSTD_IPC_TYPE_TCP] = GSTD_TYPE_TCP,
    [GSTD_IPC_TYPE_UNIX] = GSTD_TYPE_UNIX,
    [GSTD_IPC_TYPE_HTTP] = GSTD_TYPE_HTTP
  };

  const gint size = sizeof (code_description) / sizeof (gchar *);

  g_return_val_if_fail (0 <= code, GSTD_TYPE_IPC);      // TODO: Proponer un GSTD_TYPE_DEFAULT
  g_return_val_if_fail (size > code, GSTD_TYPE_IPC);

  return code_description[code];
}

GstdStatus
gstd_manager_new (Supported_IPCs supported_ipcs[], guint num_ipcs,
    GstDManager ** out)
{
  GstDManager *manager;
  GstdSession *session;
  GstdStatus ret = GSTD_LIB_OK;
  GstdIpc **ipc_array = g_malloc (num_ipcs * sizeof (GstdIpc *));

  manager = (GstDManager *) malloc (sizeof (GstDManager));
  session = gstd_session_new ("Session0");

  for (int i = 0; i < num_ipcs; i++) {
    ipc_array[i] =
        GSTD_IPC (g_object_new (gstd_supported_ipc_to_ipc (supported_ipcs[i]),
            NULL));
  }

  manager->session = session;
  manager->ipc_array = ipc_array;
  manager->num_ipcs = num_ipcs;

  *out = manager;

  return ret;
}

void
gstd_manager_init (void)
{
  g_print ("SIMPLE INIT\n");
  gst_init (NULL, NULL);
  gstd_debug_init ();
}


void
gstd_manager_init_options (void **gst_group)
{
  g_print ("OPTIONS INIT\n");
  g_return_if_fail (gst_group);
  gst_init (NULL, NULL);
  gstd_debug_init ();

  if (gst_group != NULL && *gst_group != NULL) {
    *(GOptionGroup **) gst_group = gst_init_get_option_group ();
  }

}

void
gstd_manager_ipc_options (GstDManager * manager, void **ipc_group)
{
  GOptionGroup **ipc_group_gen;
  gint i;

  ipc_group_gen = g_malloc (sizeof (ipc_group));
  g_return_if_fail (ipc_group);

  for (i = 0; i < manager->num_ipcs; i++) {
    gstd_ipc_get_option_group (manager->ipc_array[i], &ipc_group_gen[i]);
  }

  *(GOptionGroup **) ipc_group = *ipc_group_gen;
}

int
gstd_manager_ipc_start (GstDManager * manager)
{
  gboolean ipc_selected = FALSE;
  gboolean ret = TRUE;
  GstdReturnCode code;
  gint i;

  g_return_val_if_fail (manager->ipc_array, FALSE);
  g_return_val_if_fail (manager->session, FALSE);

  /* Verify if at leas one IPC mechanism was selected */
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
  gint i;

  g_return_if_fail (manager);

  /* Run stop for each IPC */
  for (i = 0; i < manager->num_ipcs; i++) {
    if (TRUE == manager->ipc_array[i]->enabled) {
      gstd_ipc_stop (manager->ipc_array[i]);
      g_object_unref (manager->ipc_array[i]);
    }
  }
}

void
gstd_manager_free (GstDManager * manager)
{
  gst_deinit ();
  gstd_assert_and_ret (NULL != manager);
  g_free (manager);
}

void
myPrint (void)
{
  g_print ("TEST: %ld\n", gstd_supported_ipc_to_ipc (GSTD_IPC_TYPE_TCP));
  g_print ("HELLO THERE!\n");
}
