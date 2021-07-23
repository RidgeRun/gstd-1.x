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

struct _GstDManager
{
  GstdSession *session;
  GstdIpc **ipc_array;
  guint num_ipcs;
  int timeout;
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
myPrint (void)
{
  g_print ("%ld\n", gstd_supported_ipc_to_ipc (GSTD_IPC_TYPE_TCP));
  g_print ("HELLO THERE!\n");
}
