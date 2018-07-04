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

#include <stdio.h>
#include <string.h>
#include <gst/gst.h>

#include "gstd_ipc.h"
#include "gstd_tcp.h"
#include "gstd_parser.h"
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"


/* Gstd TCP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_tcp_debug);
#define GST_CAT_DEFAULT gstd_tcp_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


struct _GstdTcp
{
  GstdIpc parent;
  guint base_port;
  gchar *address;
  guint num_ports;
  GSocketService *service;
};

struct _GstdTcpClass
{
  GstdIpcClass parent_class;
};

G_DEFINE_TYPE (GstdTcp, gstd_tcp, GSTD_TYPE_IPC);

enum
{
  PROP_BASE_PORT = 1,
  PROP_NUM_PORTS = 2,
  PROP_ADDRESS = 3,
  N_PROPERTIES                  // NOT A PROPERTY
};


/* VTable */

static gboolean
gstd_tcp_callback (GSocketService * service,
    GSocketConnection * connection,
    GObject * source_object, gpointer user_data);
static void gstd_tcp_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_tcp_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_tcp_dispose (GObject *);
gboolean gstd_tcp_init_get_option_group (GstdIpc * base, GOptionGroup ** group);
static gboolean gstd_tcp_add_listeners(GSocketService *service, gchar * address, gint port, GError ** error);


static void
gstd_tcp_class_init (GstdTcpClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;
  object_class->set_property = gstd_tcp_set_property;
  object_class->get_property = gstd_tcp_get_property;
  gstdipc_class->start = GST_DEBUG_FUNCPTR (gstd_tcp_start);
  gstdipc_class->stop = GST_DEBUG_FUNCPTR (gstd_tcp_stop);
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_tcp_init_get_option_group);
  object_class->dispose = gstd_tcp_dispose;

  properties[PROP_ADDRESS] =
      g_param_spec_string ("base-address",
      "Base Address",
      "The address to start listening to",
      GSTD_TCP_DEFAULT_ADDRESS,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_BASE_PORT] =
      g_param_spec_uint ("base-port",
      "Base Port",
      "The port to start listening to",
      0,
      G_MAXINT,
      GSTD_TCP_DEFAULT_PORT,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_NUM_PORTS] =
      g_param_spec_uint ("num-ports",
      "Num Ports",
      "The number of ports to open for the tcp session, starting at base-port",
      0,
      G_MAXINT,
      GSTD_TCP_DEFAULT_NUM_PORTS,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_tcp_debug, "gstdtcp", debug_color,
      "Gstd TCP category");
}

static void
gstd_tcp_init (GstdTcp * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd Tcp");
  GstdIpc *base = GSTD_IPC (self);
  self->base_port = GSTD_TCP_DEFAULT_PORT;
  self->address = g_strdup(GSTD_TCP_DEFAULT_ADDRESS);
  self->num_ports = GSTD_TCP_DEFAULT_NUM_PORTS;
  self->service = NULL;
  base->enabled = FALSE;
}

static void
gstd_tcp_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdTcp *self = GSTD_TCP (object);

  switch (property_id) {
    case PROP_ADDRESS:
      GST_DEBUG_OBJECT (self, "Returning base-address %s", self->address);
      g_value_set_string (value, self->address);
      break;
    case PROP_BASE_PORT:
      GST_DEBUG_OBJECT (self, "Returning base-port %u", self->base_port);
      g_value_set_uint (value, self->base_port);
      break;
    case PROP_NUM_PORTS:
      GST_DEBUG_OBJECT (self, "Returning number-ports %u", self->num_ports);
      g_value_set_uint (value, self->num_ports);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_tcp_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdTcp *self = GSTD_TCP (object);

  switch (property_id) {
    case PROP_ADDRESS:
      {
      const gchar * address;
      GST_DEBUG_OBJECT (self, "Changing base-port current value: %s",
          self->address);

      address = g_value_get_string (value);
      g_free(self->address);
      self->address = g_strdup(address);
      GST_DEBUG_OBJECT (self, "Value changed %s", self->address);
      break;
      }
    case PROP_BASE_PORT:
      GST_DEBUG_OBJECT (self, "Changing base-port current value: %u",
          self->base_port);
      self->base_port = g_value_get_uint (value);
      GST_DEBUG_OBJECT (self, "Value changed %u", self->base_port);
      break;
    case PROP_NUM_PORTS:
      GST_DEBUG_OBJECT (self, "Changing num-ports current value: %u",
          self->num_ports);
      self->num_ports = g_value_get_uint (value);
      GST_DEBUG_OBJECT (self, "Value changed %u", self->num_ports);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}


static void
gstd_tcp_dispose (GObject * object)
{
  GstdTcp *self = GSTD_TCP (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd TCP");

  if(self->address)
    g_free(self->address);

  if (self->service) {
    self->service = NULL;
  }

  G_OBJECT_CLASS (gstd_tcp_parent_class)->dispose (object);
}



static gboolean
gstd_tcp_callback (GSocketService * service,
    GSocketConnection * connection, GObject * source_object, gpointer user_data)
{

  GstdSession *session = GSTD_SESSION (user_data);
  GInputStream *istream;
  GOutputStream *ostream;
  gint read;
  const guint size = 1024*1024;
  gchar *output = NULL;
  gchar *response;
  gchar *message;
  GstdReturnCode ret;
  const gchar *description = NULL;

  g_return_val_if_fail (session, TRUE);

  istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  message = g_malloc (size);

  read = g_input_stream_read (istream, message, size, NULL, NULL);
  message[read] = '\0';

  ret = gstd_parser_parse_cmd (session, message, &output); // in the parser
  g_free (message);

  /* Prepend the code to the output */
  description = gstd_return_code_to_string (ret);
  response =
      g_strdup_printf
      ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");
  g_free (output);

  g_output_stream_write (ostream, response, strlen (response) + 1, NULL, NULL);
  g_free (response);

  return FALSE;
}

GstdReturnCode
gstd_tcp_start (GstdIpc * base, GstdSession * session)
{
  guint debug_color;
  GError *error = NULL;
  GstdTcp *self = GSTD_TCP (base);
  GSocketService **service;
  guint16 port = self->base_port;
  gchar *address = self->address;
  guint i;
  if (!base->enabled) {
    GST_DEBUG_OBJECT (self, "TCP not enabled, skipping");
    goto out;
  }

  GST_DEBUG_OBJECT (self, "Starting TCP");

  if (!gstd_tcp_debug) {
    /* Initialize debug category with nice colors */
    debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
    GST_DEBUG_CATEGORY_INIT (gstd_tcp_debug, "gstdtcp", debug_color,
        "Gstd TCP category");
  }
  // Close any existing connection
  gstd_tcp_stop (base);

  service = &self->service;
  *service = g_threaded_socket_service_new (self->num_ports);

  for (i = 0; i < self->num_ports; i++) {
    gstd_tcp_add_listeners(*service, address, port + i, &error);
    if (error)
      goto noconnection;
  }

  /* listen to the 'incoming' signal */
  g_signal_connect (*service, "run", G_CALLBACK (gstd_tcp_callback), session);

  /* start the socket service */
  g_socket_service_start (*service);


out:
  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (session, "%s", error->message);
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return GSTD_NO_CONNECTION;
  }
}

GstdReturnCode
gstd_tcp_stop (GstdIpc * base)
{
  GstdTcp *self = GSTD_TCP (base);
  GSocketService **service;
  GstdSession *session = base->session;

  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Entering TCP stop ");
  if (self->service) {
    service = &self->service;
    GSocketListener *listener = G_SOCKET_LISTENER (*service);
    if (*service) {
      GST_INFO_OBJECT (session, "Closing TCP connection for %s",
          GSTD_OBJECT_NAME (session));
      g_socket_listener_close (listener);
      g_socket_service_stop (*service);
      g_object_unref (*service);
      *service = NULL;
    }
  }
  return GSTD_EOK;
}



gboolean
gstd_tcp_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdTcp *self = GSTD_TCP (base);
  GST_DEBUG_OBJECT (self, "TCP init group callback ");
  GOptionEntry tcp_args[] = {
    {"enable-tcp-protocol", 't', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given TCP ports ", NULL}
    ,
    {"address", 'a', 0, G_OPTION_ARG_STRING, &self->address,
          "Attach to the server starting through a given address (default 127.0.0.1)",
        "address"}
    ,
    {"base-port", 'p', 0, G_OPTION_ARG_INT, &self->base_port,
          "Attach to the server starting through a given port (default 5000)",
        "base-port"}
    ,
    {"num-ports", 'n', 0, G_OPTION_ARG_INT, &self->num_ports,
          "Number of ports to use starting at base-port (default 1)",
        "num-ports"}
    ,
    {NULL}
  };
  *group = g_option_group_new ("gstd-tcp", ("TCP Options"),
      ("Show TCP Options"), NULL, NULL);

  g_option_group_add_entries (*group, tcp_args);
  return TRUE;
}

static gboolean
gstd_tcp_add_listeners(GSocketService *service, gchar * address, gint port, GError ** error)
{
  GSocketAddress * sa;
  gboolean ret = TRUE;

  g_return_val_if_fail(service, FALSE);
  g_return_val_if_fail(address, FALSE);
  g_return_val_if_fail(error != NULL, FALSE);

  sa = g_inet_socket_address_new_from_string(address, port);

  if(g_socket_listener_add_address(G_SOCKET_LISTENER(service), sa,
    G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, NULL, error)
      == FALSE ){
    ret = FALSE;
  }

  g_object_unref(sa);

  return ret;
}
