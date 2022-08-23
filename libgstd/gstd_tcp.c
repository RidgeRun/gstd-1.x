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

#include <stdio.h>
#include <string.h>
#include <gst/gst.h>

#include "gstd_ipc.h"
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"

#include "gstd_tcp.h"

/* Gstd TCP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_tcp_debug);
#define GST_CAT_DEFAULT gstd_tcp_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


struct _GstdTcp
{
  GstdSocket parent;
  guint base_port;
  gchar *address;
  guint num_ports;
  gint max_threads;
  GSocketService *service;
};

struct _GstdTcpClass
{
  GstdSocketClass parent_class;
};

G_DEFINE_TYPE (GstdTcp, gstd_tcp, GSTD_TYPE_SOCKET);

/* VTable */

static void gstd_tcp_dispose (GObject *);
static GstdReturnCode gstd_tcp_create_socket_service (GstdSocket * base,
    GSocketService ** service);
static gboolean gstd_tcp_init_get_option_group (GstdIpc * base,
    GOptionGroup ** group);
static gboolean gstd_tcp_add_listeners (GSocketService * service,
    gchar * address, gint port, GError ** error);


static void
gstd_tcp_class_init (GstdTcpClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  GstdSocketClass *socket_class = GSTD_SOCKET_CLASS (klass);
  guint debug_color;
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_tcp_init_get_option_group);
  socket_class->create_socket_service =
      GST_DEBUG_FUNCPTR (gstd_tcp_create_socket_service);
  object_class->dispose = gstd_tcp_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_tcp_debug, "gstdtcp", debug_color,
      "Gstd TCP category");
}

static void
gstd_tcp_init (GstdTcp * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd Tcp");
  self->base_port = GSTD_TCP_DEFAULT_PORT;
  self->address = g_strdup (GSTD_TCP_DEFAULT_ADDRESS);
  self->num_ports = GSTD_TCP_DEFAULT_NUM_PORTS;
  self->max_threads = GSTD_TCP_DEFAULT_MAX_THREADS;
}

static void
gstd_tcp_dispose (GObject * object)
{
  GstdTcp *self = GSTD_TCP (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd TCP");

  if (self->address)
    g_free (self->address);

  G_OBJECT_CLASS (gstd_tcp_parent_class)->dispose (object);
}

GstdReturnCode
gstd_tcp_create_socket_service (GstdSocket * base, GSocketService ** service)
{
  GError *error = NULL;
  GstdTcp *self = GSTD_TCP (base);
  guint16 port = self->base_port;
  gchar *address = self->address;
  guint i;

  GST_DEBUG_OBJECT (self, "Getting TCP Socket address");

  *service = g_threaded_socket_service_new (self->max_threads);

  for (i = 0; i < self->num_ports; i++) {
    gstd_tcp_add_listeners (*service, address, port + i, &error);
    if (error)
      goto noconnection;
  }

  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (self, "%s", error->message);
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    g_socket_service_stop (*service);
    *service = NULL;
    return GSTD_NO_CONNECTION;
  }
}

gboolean
gstd_tcp_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdTcp *self = GSTD_TCP (base);
  GOptionEntry tcp_args[] = {
    {"enable-tcp-protocol", 't', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given TCP ports ", NULL}
    ,
    {"tcp-address", 'a', 0, G_OPTION_ARG_STRING, &self->address,
          "Attach to the server starting through a given address (default 127.0.0.1)",
        "tcp-address"}
    ,
    {"tcp-base-port", 'p', 0, G_OPTION_ARG_INT, &self->base_port,
          "Attach to the server starting through a given port (default 5000)",
        "tcp-base-port"}
    ,
    {"tcp-num-ports", 'n', 0, G_OPTION_ARG_INT, &self->num_ports,
          "Number of ports to use starting at base-port (default 1)",
        "tcp-num-ports"}
    ,
    {"tcp-max-threads", 'm', 0, G_OPTION_ARG_INT, &self->max_threads,
          "Max number of allowed threads to process simultaneous requests. -1 "
          "means unlimited (default -1)",
        "tcp-max-threads"}
    ,
    {NULL}
  };
  GST_DEBUG_OBJECT (self, "TCP init group callback ");
  *group = g_option_group_new ("gstd-tcp", ("TCP Options"),
      ("Show TCP Options"), NULL, NULL);

  g_option_group_add_entries (*group, tcp_args);
  return TRUE;
}

static gboolean
gstd_tcp_add_listeners (GSocketService * service, gchar * address, gint port,
    GError ** error)
{
  GSocketAddress *sa;
  gboolean ret = TRUE;

  g_return_val_if_fail (service, FALSE);
  g_return_val_if_fail (address, FALSE);
  g_return_val_if_fail (error != NULL, FALSE);

  sa = g_inet_socket_address_new_from_string (address, port);

  if (g_socket_listener_add_address (G_SOCKET_LISTENER (service), sa,
          G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, NULL, error)
      == FALSE) {
    ret = FALSE;
  }

  g_object_unref (sa);

  return ret;
}
