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

/**
 * SECTION:gstd_ipc
 * @short_description: GstdIPC Object
 * @title: GstdIPC Object
 * @see_also:#GstdObject
 * @include: gstd/gstd.h
 *
 * # Introduction #
 *
 * A #GstdIpc encapsulates a GStreamer Daemon IPC. It holds the
 * structure of callbacks and provides mechanisms to the user to 
 * interact with them. This class works as a proxy class for IPC
 * so that gstd can bring them up according to what the user
 * specifies in the cmdline. 
 * A #GstdIpc is created and deleted as any other GObject:
 * |[<!-- language="C" -->
 * #include <gstd/gstd.h>
 * 
 * gchar *name;
 * GstdIpc *gstd;
 *
 * gstd = g_object_new (GSTD_TYPE_IPC, "name", NULL);
 * g_object_get (G_OBJECT(gstd), "name", &name, NULL);
 * g_print ("The IPC name is \"%s\"", name);
 *
 * g_free (name);
 * g_object_unref (gstd);
 * ]|
 * 
 */

#ifndef __GSTD_IPC___
#define __GSTD_IPC___

#include <glib.h>

#include "gstd_return_codes.h"
#include "gstd_object.h"
#include "gstd_pipeline.h"
#include "gstd_session.h"
#include "gstd_parser.h"

G_BEGIN_DECLS
#define GSTD_TYPE_IPC \
  (gstd_ipc_get_type())
#define GSTD_IPC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_IPC,GstdIpc))
#define GSTD_IPC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_IPC,GstdIpcClass))
#define GSTD_IS_IPC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_IPC))
#define GSTD_IS_IPC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_IPC))
#define GSTD_IPC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_IPC, GstdIpcClass))
typedef struct _GstdIpc GstdIpc;
typedef struct _GstdIpcClass GstdIpcClass;

struct _GstdIpc
{
  GstdObject parent;
  /**
   * A boolean used to enable the IPC
   */
  gboolean enabled;
    /**
   * A reference to the GstSession used f
   * used for the IPC
   */
  GstdSession *session;
};

struct _GstdIpcClass
{
  GstdObjectClass parent_class;

    GstdReturnCode (*start) (GstdIpc *, GstdSession *);

    GstdReturnCode (*stop) (GstdIpc *);

    gboolean (*get_option_group) (GstdIpc *, GOptionGroup **);

};

GType gstd_ipc_get_type (void);

gboolean gstd_ipc_get_option_group (GstdIpc *, GOptionGroup **);
GstdReturnCode gstd_ipc_start (GstdIpc *, GstdSession *);
GstdReturnCode gstd_ipc_stop (GstdIpc *);
G_END_DECLS
#endif //__GSTD_IPC___
