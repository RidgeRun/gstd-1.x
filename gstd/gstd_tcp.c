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
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"

/* Gstd TCP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_tcp_debug);
#define GST_CAT_DEFAULT gstd_tcp_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

#define check_argument(arg, code) \
    if (NULL == (arg)) return (code)

struct _GstdTcp
{
  GstdIpc parent;
  guint base_port;
  guint num_ports;
  GSocketService *service;
};

struct _GstdTcpClass
{
  GstdIpcClass parent_class;
};


typedef GstdReturnCode GstdTCPFunc (GstdSession *, gchar *, gchar *, gchar **);
typedef struct _GstdTCPCmd
{
  gchar *cmd;
  GstdTCPFunc *callback;
} GstdTCPCmd;


G_DEFINE_TYPE (GstdTcp, gstd_tcp, GSTD_TYPE_IPC);

enum
{
  PROP_BASE_PORT = 1,
  PROP_NUM_PORTS = 2,
  N_PROPERTIES                  // NOT A PROPERTY
};


/* VTable */

static gboolean
gstd_tcp_callback (GSocketService * service,
    GSocketConnection * connection,
    GObject * source_object, gpointer user_data);
static GstdReturnCode
gstd_tcp_parse_cmd (GstdSession * session, const gchar * cmd,
    gchar ** response);
static GstdReturnCode gstd_tcp_parse_raw_cmd (GstdSession * session,
    gchar * action, gchar * args, gchar ** response);
static GstdReturnCode gstd_tcp_create (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_tcp_read (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** reponse);
static GstdReturnCode gstd_tcp_update (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_tcp_delete (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_tcp_pipeline_create (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_pipeline_delete (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_pipeline_play (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_pipeline_pause (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_pipeline_stop (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_element_set (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_element_get (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_list_pipelines (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_list_elements (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_list_properties (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_tcp_bus_read (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_bus_filter (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_bus_timeout (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_event_eos (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_event_seek (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_event_flush_start (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_event_flush_stop (GstdSession*, gchar *, gchar *,
    gchar **);

static GstdReturnCode gstd_tcp_debug_enable (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_debug_threshold (GstdSession*, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_tcp_debug_color (GstdSession*, gchar *, gchar *,
    gchar **);

static void gstd_tcp_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_tcp_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_tcp_dispose (GObject *);
gboolean gstd_tcp_init_get_option_group (GstdIpc * base, GOptionGroup ** group);

static GstdTCPCmd cmds[] = {
  {"create", gstd_tcp_parse_raw_cmd},
  {"read", gstd_tcp_parse_raw_cmd},
  {"update", gstd_tcp_parse_raw_cmd},
  {"delete", gstd_tcp_parse_raw_cmd},

  {"pipeline_create", gstd_tcp_pipeline_create},
  {"pipeline_delete", gstd_tcp_pipeline_delete},
  {"pipeline_play", gstd_tcp_pipeline_play},
  {"pipeline_pause", gstd_tcp_pipeline_pause},
  {"pipeline_stop", gstd_tcp_pipeline_stop},

  {"element_set", gstd_tcp_element_set},
  {"element_get", gstd_tcp_element_get},

  {"list_pipelines", gstd_tcp_list_pipelines},
  {"list_elements", gstd_tcp_list_elements},
  {"list_properties", gstd_tcp_list_properties},

  {"bus_read", gstd_tcp_bus_read},
  {"bus_filter", gstd_tcp_bus_filter},
  {"bus_timeout", gstd_tcp_bus_timeout},

  {"event_eos", gstd_tcp_event_eos},
  {"event_seek", gstd_tcp_event_seek},
  {"event_flush_start", gstd_tcp_event_flush_start},
  {"event_flush_stop", gstd_tcp_event_flush_stop},

  {"debug_enable", gstd_tcp_debug_enable},
  {"debug_threshold", gstd_tcp_debug_threshold},
  {"debug_color", gstd_tcp_debug_color},

  {NULL}
};

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

  ret = gstd_tcp_parse_cmd (session, message, &output);
  g_free (message);

  /* Prepend the code to the output */
  description = gstd_return_code_to_string(ret);
  response =
      g_strdup_printf ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}", ret, description,
      output ? output : "null");
  g_free (output);

  g_output_stream_write (ostream, response, strlen(response)+1, NULL, NULL);
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

    g_socket_listener_add_inet_port (G_SOCKET_LISTENER (*service),
        port + i, NULL /* G_OBJECT(session) */ , &error);
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

static GstdReturnCode
gstd_tcp_create (GstdSession * session, GstdObject * obj, gchar * args,
    gchar ** response)
{
  gchar **tokens = NULL;
  gchar *name;
  gchar *description;
  GstdObject *new;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (G_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);

  // This may mean a potential leak
  g_warn_if_fail (!*response);

  GST_FIXME_OBJECT (session,
      "Currently hardcoded to create pipelines and events, we must be "
      "generic enough to create any type of object");

  // Tokens has the form {<name>, <description>}
  if (NULL == args) {
    name = NULL;
    description = NULL;
  } else {
    tokens = g_strsplit (args, " ", 2);
    name = tokens[0];
    description = tokens[1];
  }

  if (NULL == name) {
    /* No name provided, hence no desciption either, but it may contain garbage */
    description = NULL;
  }

  ret = gstd_object_create (obj, name, description);
  if (ret)
    goto out;

  gstd_object_read (obj, name, &new);

  if (NULL != new) {
    gstd_object_to_string (new, response);
    g_object_unref (new);
  }

out:
  {
    if (tokens)
      g_strfreev (tokens);
    return ret;
  }
}

static GstdReturnCode
gstd_tcp_read (GstdSession * session, GstdObject * obj, gchar * args,
    gchar ** response)
{
  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);

  // This may mean a potential leak
  g_warn_if_fail (!*response);

  // Print the raw object
  return gstd_object_to_string (obj, response);
}

static GstdReturnCode
gstd_tcp_update (GstdSession * session, GstdObject * obj, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);

  if (!args) {
    GST_ERROR_OBJECT (obj, "No argument provided for update");
    ret = GSTD_BAD_VALUE;
    goto out;
  }
  *response = NULL;

  ret = gstd_object_update (obj, args);
  if (ret) {
    goto out;
  }

  /* Serialize the updated object */
  gstd_object_to_string (obj, response);
 out:
  {
    return ret;
  }
}

static GstdReturnCode
gstd_tcp_delete (GstdSession * session, GstdObject * obj, gchar * args,
    gchar ** response)
{
  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  *response = NULL;

  return gstd_object_delete (obj, args);
}

static GstdReturnCode
gstd_tcp_parse_raw_cmd (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  gchar **tokens;
  gchar *uri, *rest;
  GstdObject *node;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (action, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*response);


  tokens = g_strsplit (args, " ", 2);
  uri = tokens[0];
  rest = tokens[1];

  // Alias the empty string to the base
  if (!uri)
    uri = "/";

  ret = gstd_get_by_uri (session, uri, &node);
  if (ret || NULL == node) {
    goto out;
  }

  if (!g_ascii_strcasecmp ("CREATE", action)) {
    ret = gstd_tcp_create (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("READ", action)) {
    ret = gstd_tcp_read (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("UPDATE", action)) {
    ret = gstd_tcp_update (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("DELETE", action)) {
    ret = gstd_tcp_delete (session, node, rest, response);
  } else {
    GST_ERROR_OBJECT (session, "Unknown command \"%s\"", action);
    ret = GSTD_BAD_COMMAND;
  }

  g_object_unref (node);

out:
  {
    g_strfreev (tokens);
    return ret;
  }
}

static GstdReturnCode
gstd_tcp_parse_cmd (GstdSession * session, const gchar * cmd, gchar ** response)
{
  gchar **tokens;
  gchar *action, *args;
  GstdTCPCmd *cb;
  GstdReturnCode ret = GSTD_BAD_COMMAND;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (cmd, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*response);

  tokens = g_strsplit (cmd, " ", 2);
  action = tokens[0];
  args = tokens[1];

  cb = cmds;
  while (cb) {
    if (!g_ascii_strcasecmp (cb->cmd, action)) {
      ret = cb->callback (session, action, args, response);
      break;
    }
    cb++;
  }

  if (ret == GSTD_BAD_COMMAND)
    GST_ERROR_OBJECT (session, "Unknown command \"%s\"", action);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_pipeline_create (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines %s", args ? args : "");

  ret = gstd_tcp_parse_raw_cmd (session, "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_pipeline_delete (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines %s", args);
  ret = gstd_tcp_parse_raw_cmd (session, "delete", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_pipeline_play (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state playing", args);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_pipeline_pause (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state paused", args);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_pipeline_stop (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state null", args);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_element_set (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 4);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);
  check_argument (tokens[2], GSTD_BAD_COMMAND);
  check_argument (tokens[3], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/properties/%s %s",
      tokens[0], tokens[1], tokens[2], tokens[3]);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_element_get (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 3);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);
  check_argument (tokens[2], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/properties/%s",
      tokens[0], tokens[1], tokens[2]);
  ret = gstd_tcp_parse_raw_cmd (session, "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_list_pipelines (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines");
  ret = gstd_tcp_parse_raw_cmd (session, "read", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_list_elements (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/elements/", args);
  ret = gstd_tcp_parse_raw_cmd (session, "read", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_list_properties (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/properties", tokens[0], tokens[1]);
  ret = gstd_tcp_parse_raw_cmd (session, "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_bus_read (GstdSession *session, gchar * action,
    gchar *pipeline, gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/bus/message", pipeline);
  ret = gstd_tcp_parse_raw_cmd (session, "read", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_bus_filter (GstdSession *session, gchar *action,
    gchar *args, gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/bus/types %s", tokens[0], tokens[1]);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_bus_timeout (GstdSession *session, gchar *action, gchar *args,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/bus/timeout %s", tokens[0], tokens[1]);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_event_eos (GstdSession *session, gchar *action, gchar *pipeline,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/event eos", pipeline);
  ret = gstd_tcp_parse_raw_cmd (session, "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_event_seek (GstdSession *session, gchar *action, gchar *args,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  // We don't check for the second token since we want to allow defaults

  uri = g_strdup_printf ("/pipelines/%s/event seek %s", tokens[0], tokens[1]);
  ret = gstd_tcp_parse_raw_cmd (session, "create", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_event_flush_start (GstdSession *session, gchar *action, gchar *pipeline,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/event flush_start", pipeline);
  ret = gstd_tcp_parse_raw_cmd (session, "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_event_flush_stop (GstdSession *session, gchar *action, gchar *args,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  // We don't check for the second token since we want to allow defaults

  uri = g_strdup_printf ("/pipelines/%s/event flush_stop %s", tokens[0], tokens[1]);
  ret = gstd_tcp_parse_raw_cmd (session, "create", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_tcp_debug_enable (GstdSession *session, gchar *action, gchar *enabled,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (enabled, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/enable %s", enabled);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_debug_threshold (GstdSession *session, gchar *action, gchar *threshold,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (threshold, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/threshold %s", threshold);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_tcp_debug_color (GstdSession *session, gchar *action, gchar *colored,
    gchar **response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (colored, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/color %s", colored);
  ret = gstd_tcp_parse_raw_cmd (session, "update", uri, response);

  g_free (uri);

  return ret;
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
