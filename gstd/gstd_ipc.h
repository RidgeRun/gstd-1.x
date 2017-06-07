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
