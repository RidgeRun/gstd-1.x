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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <gio/gunixsocketaddress.h>

#include "gstd_unix.h"

/* Gstd TCP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_unix_debug);
#define GST_CAT_DEFAULT gstd_unix_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


struct _GstdUnix
{
  GstdSocket parent;
  gchar *unix_path;
  guint num_ports;
  GSocketService *service;
};

struct _GstdUnixClass
{
  GstdSocketClass parent_class;
};

G_DEFINE_TYPE (GstdUnix, gstd_unix, GSTD_TYPE_SOCKET);

/* VTable */

static void gstd_unix_dispose (GObject *);
static GstdReturnCode gstd_unix_create_socket_service (GstdSocket * base,
    GSocketService ** service);
static gboolean gstd_unix_init_get_option_group (GstdIpc * base,
    GOptionGroup ** group);


static void
gstd_unix_class_init (GstdUnixClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  GstdSocketClass *socket_class = GSTD_SOCKET_CLASS (klass);
  guint debug_color;
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_unix_init_get_option_group);
  socket_class->create_socket_service =
      GST_DEBUG_FUNCPTR (gstd_unix_create_socket_service);
  object_class->dispose = gstd_unix_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_unix_debug, "gstdunix", debug_color,
      "Gstd UNIX category");
}

static void
gstd_unix_set_path (GstdUnix * self, const gchar * path)
{
  g_free (self->unix_path);

  if (path != NULL)
    self->unix_path = g_strdup (path);
  else
    self->unix_path = NULL;
}

static void
gstd_unix_init (GstdUnix * self)
{
  gchar *default_path;
  GST_INFO_OBJECT (self, "Initializing gstd Unix");

  default_path =
      g_strdup_printf ("%s/%s", GSTD_RUN_STATE_DIR,
      GSTD_UNIX_DEFAULT_BASE_NAME);
  gstd_unix_set_path (self, default_path);
  g_free (default_path);
  self->num_ports = GSTD_UNIX_DEFAULT_NUM_PORTS;

}

static void
gstd_unix_dispose (GObject * object)
{
  GstdUnix *self = GSTD_UNIX (object);
  GstdIpc *parent = GSTD_IPC (object);
  guint i;

  GST_INFO_OBJECT (object, "Deinitializing gstd UNIX");

  if (parent->enabled) {
    for (i = 0; i < self->num_ports; i++) {
      gchar *path_name = g_strdup_printf ("%s_%d", self->unix_path, i);

      if (unlink (path_name) != 0) {
        GST_ERROR_OBJECT (object, "Unable to delete UNIX path (%s)",
            strerror (errno));
      }
      g_free (path_name);
    }
  }

  if (NULL != self->unix_path) {
    g_free (self->unix_path);
    self->unix_path = NULL;
  }

  G_OBJECT_CLASS (gstd_unix_parent_class)->dispose (object);
}

GstdReturnCode
gstd_unix_create_socket_service (GstdSocket * base, GSocketService ** service)
{
  GError *error = NULL;
  GstdUnix *self = GSTD_UNIX (base);
  gchar *path = self->unix_path;
  guint i;

  GST_DEBUG_OBJECT (self, "Getting UNIX Socket address");

  *service = g_threaded_socket_service_new (self->num_ports);

  for (i = 0; i < self->num_ports; i++) {
    GSocketAddress *address;
    gchar *path_name = g_strdup_printf ("%s_%d", path, i);

    address = g_unix_socket_address_new (path_name);
    g_free (path_name);

    g_socket_listener_add_address (G_SOCKET_LISTENER (*service),
        address,
        G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, NULL, NULL, &error);
    if (error)
      goto noconnection;
  }
  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (self, "%s", error->message);
    g_error_free (error);
    g_socket_service_stop (*service);
    *service = NULL;
    return GSTD_NO_CONNECTION;
  }
}

gboolean
gstd_unix_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdUnix *self = GSTD_UNIX (base);
  GOptionEntry unix_args[] = {
    {"enable-unix-protocol", 'u', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given UNIX socket ", NULL}
    ,
    {"unix-base-path", 'b', 0, G_OPTION_ARG_STRING, &self->unix_path,
          "Attach to the server using the given path (default /usr/local/var/run/gstd/gstd_default_unix_socket), "
          "a '_<port_number>' is appended to this path to create the ports, for instance if only one port is created, its path will be /usr/local/var/run/gstd/gstd_default_unix_socket_0",
        "unix-path"}
    ,
    {"unix-num-ports", 'c', 0, G_OPTION_ARG_INT, &self->num_ports,
          "Number of ports to use starting at base-port (default 1)",
        "unix-num-ports"}
    ,
    {NULL}
  };
  GST_DEBUG_OBJECT (self, "UNIX init group callback ");
  *group = g_option_group_new ("gstd-unix", ("UNIX Options"),
      ("Show UNIX Options"), NULL, NULL);

  g_option_group_add_entries (*group, unix_args);
  return TRUE;
}
