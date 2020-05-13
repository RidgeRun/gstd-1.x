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
#include <libsoup/soup.h>

#include "gstd_ipc.h"
#include "gstd_http.h"
#include "gstd_parser.h"
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"


/* Gstd HTTP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_http_debug);
#define GST_CAT_DEFAULT gstd_http_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


struct _GstdHttp
{
  GstdIpc parent;
  guint base_port;
  gchar *address;
  guint num_ports;
  gint max_threads;
};

struct _GstdHttpClass
{
  GstdIpcClass parent_class;
};

G_DEFINE_TYPE (GstdHttp, gstd_http, GSTD_TYPE_IPC);

/* VTable */

static void gstd_http_dispose (GObject *);
static GstdReturnCode gstd_http_start (GstdIpc * base, GstdSession * session);
static GstdReturnCode gstd_http_stop (GstdIpc * base);
static gboolean gstd_http_init_get_option_group (GstdIpc * base, GOptionGroup ** group);

static void
gstd_http_class_init (GstdHttpClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  guint debug_color;
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_http_init_get_option_group);
  gstdipc_class->start =
      GST_DEBUG_FUNCPTR (gstd_http_start);
  object_class->dispose = gstd_http_dispose;
  gstdipc_class->stop =
      GST_DEBUG_FUNCPTR (gstd_http_stop);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_http_debug, "gstdhttp", debug_color,
      "Gstd HTTP category");
}

static void
gstd_http_init (GstdHttp * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd Http");
  self->base_port = GSTD_HTTP_DEFAULT_PORT;
  self->address = g_strdup(GSTD_HTTP_DEFAULT_ADDRESS);
  self->num_ports = GSTD_HTTP_DEFAULT_NUM_PORTS;
  self->max_threads = GSTD_HTTP_DEFAULT_MAX_THREADS;
}

static void
gstd_http_dispose (GObject * object)
{
  GstdHttp *self = GSTD_HTTP (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd HTTP");

  if(self->address)
    g_free(self->address);

  G_OBJECT_CLASS (gstd_http_parent_class)->dispose (object);
}

static void
do_get (SoupServer *server, SoupMessage *msg, GstdSession *session)
{
  gchar *response;
  gchar *message;
  SoupURI *address;
  GstdReturnCode ret;
  gchar *output = NULL;
  const gchar *description = NULL;

  address = soup_message_get_uri(msg);
  g_print("Message:   %s\n",soup_uri_get_path(address));
  message = g_strdup_printf
    ("read %s" ,soup_uri_get_path(address));
  ret = gstd_parser_parse_cmd (session, message, &output); // in the parser
  // Prepend the code to the output 
  description = gstd_return_code_to_string (ret);
  response =
    g_strdup_printf
    ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");
      
  g_print("response:%s\n",response);
  soup_message_set_status (msg, SOUP_STATUS_OK);
  soup_message_set_response (msg, "application/json",SOUP_MEMORY_COPY, response,
              strlen(response));

  g_free (output);
  g_free(response);
  g_free(message);

  return;
}

static void
do_post (SoupServer *server, SoupMessage *msg, GHashTable *query, GstdSession *session)
{
  gchar *response;
  gchar *message;
  gchar *name;
  gchar *description_pipe;
  SoupURI *address;
  GstdReturnCode ret;
  gchar *output = NULL;
  const gchar *description = NULL;

  address = soup_message_get_uri(msg);
  g_print("Message:   %s\n",soup_uri_get_path(address));
  query = soup_form_decode (soup_uri_get_query (address));
  name = g_hash_table_lookup (query, "name");
  if (name){
    name = g_strdup (name);
    }
  description_pipe = g_hash_table_lookup (query, "description");
  if (description_pipe){
    description_pipe = g_strdup (description_pipe);
    }
  message = g_strdup_printf
    ("create %s %s %s" ,soup_uri_get_path(address),name,description_pipe);
  ret = gstd_parser_parse_cmd (session, message, &output); // in the parser
  // Prepend the code to the output 
  description = gstd_return_code_to_string (ret);
  response =
    g_strdup_printf
    ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");
  
  g_print("response:%s\n",response);
  soup_message_set_response (msg, "application/json",SOUP_MEMORY_COPY, response,
              strlen(response));
  g_free (output);
  g_free(response);
  g_free(message);
  g_free(name);
  g_free(description_pipe);
  
  soup_message_set_status (msg, SOUP_STATUS_OK);
  return;
}

static void
do_put (SoupServer *server, SoupMessage *msg, GHashTable *query, GstdSession *session)
{
  gchar *response;
  gchar *message;
  gchar *name;
  gchar *description_pipe;
  SoupURI *address;
  GstdReturnCode ret;
  gchar *output = NULL;
  const gchar *description = NULL;

  address = soup_message_get_uri(msg);
  query = soup_form_decode (soup_uri_get_query (address));
  name = g_hash_table_lookup (query, "name");
  if (name){
    name = g_strdup (name);
  }
  description_pipe = g_hash_table_lookup (query, "description");
  if (description_pipe){
    description_pipe = g_strdup (description_pipe);
    }
  message = g_strdup_printf
    ("update %s %s" ,soup_uri_get_path(address),name);
  ret = gstd_parser_parse_cmd (session, message, &output); // in the parser
  // Prepend the code to the output 
  description = gstd_return_code_to_string (ret);
  response =
    g_strdup_printf
    ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");

  g_print("response:%s\n",response);
  soup_message_set_response (msg, "application/json",SOUP_MEMORY_COPY, response,
              strlen(response));
  g_free (output);
  g_free(response);
  g_free(message);
  g_free(name);
  g_free(description_pipe);

  if(ret == GSTD_EOK){
    soup_message_set_status (msg, SOUP_STATUS_OK);
  }else{
    soup_message_set_status (msg, SOUP_STATUS_BAD_REQUEST);
  }
  
  return;
}

static void
do_delete (SoupServer *server, SoupMessage *msg, GHashTable *query, GstdSession *session)
{
  gchar *response;
  gchar *message;
  gchar *name;
  SoupURI *address;
  GstdReturnCode ret;
  gchar *output = NULL;
  const gchar *description = NULL;

  address = soup_message_get_uri(msg);
  g_print("Message:   %s\n",soup_uri_get_path(address));
  query = soup_form_decode (soup_uri_get_query (address));
  name = g_hash_table_lookup (query, "name");
  if (name){
    name = g_strdup (name);
  }
  message = g_strdup_printf
    ("delete %s %s" ,soup_uri_get_path(address),name);
  ret = gstd_parser_parse_cmd (session, message, &output); // in the parser
  // Prepend the code to the output 
  description = gstd_return_code_to_string (ret);
  response =
    g_strdup_printf
    ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");

  g_print("response:%s\n",response);
  soup_message_set_response (msg, "application/json",SOUP_MEMORY_COPY, response,
              strlen(response));
  soup_message_set_status (msg, SOUP_STATUS_OK);

  g_free (output);
  g_free(response);
  g_free(message);
  g_free(name);
  

  return;
}

static void
server_callback (SoupServer *server, SoupMessage *msg,
     const char *path, GHashTable *query,
     SoupClientContext *context, gpointer data)
{
  SoupMessageHeadersIter iter;
  GstdSession *session;

  soup_message_set_flags (msg,SOUP_MESSAGE_NEW_CONNECTION);
  g_print ("%s %s HTTP/1.%d\n", msg->method, path,
        soup_message_get_http_version (msg));
  session = GSTD_SESSION (data);
  soup_message_headers_iter_init (&iter, msg->request_headers);
  if (msg->request_body->length){
    g_print ("%s\n", msg->request_body->data);
  }

  if (msg->method == SOUP_METHOD_GET){
    do_get (server, msg, session);
  }
  else if(msg->method == SOUP_METHOD_POST){
    do_post (server, msg,query,session);
  }
  else if(msg->method == SOUP_METHOD_PUT){
    do_put (server, msg,query,session);
  }
  else if(msg->method == SOUP_METHOD_DELETE){
    do_delete (server, msg,query,session);
  }
  else{
    soup_message_set_status (msg, SOUP_STATUS_NOT_IMPLEMENTED);
  }

  g_print ("  -> %d %s\n\n", msg->status_code, msg->reason_phrase);
}

GstdReturnCode
gstd_http_start (GstdIpc * base, GstdSession * session)
{
  GError *error = NULL;
  GstdHttp *self = GSTD_HTTP (base);
  guint16 port = self->base_port;
  SoupServer *server;

  gstd_http_stop (base);

  GST_DEBUG_OBJECT (self, "Getting HTTP address");
  server = soup_server_new(SOUP_SERVER_SERVER_HEADER,"HTTP-Server",NULL);

  soup_server_listen_all (server, port+1, 0, &error);
  if (error){
    goto noconnection;
  }
  soup_server_add_handler (server, NULL, server_callback, session, NULL);
	g_print ("\nWaiting for requests...\n");

  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (self, "%s", error->message);
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return GSTD_NO_CONNECTION;
  }
}

gboolean
gstd_http_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdHttp *self = GSTD_HTTP (base);
  GOptionEntry http_args[] = {
    {"enable-http-protocol", 't', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given HTTP ports ", NULL}
    ,
    {"http-address", 'a', 0, G_OPTION_ARG_STRING, &self->address,
          "Attach to the server starting through a given address (default 127.0.0.1)",
        "http-address"}
    ,
    {"http-base-port", 'p', 0, G_OPTION_ARG_INT, &self->base_port,
          "Attach to the server starting through a given port (default 5000)",
        "http-base-port"}
    ,
    {"http-num-ports", 'n', 0, G_OPTION_ARG_INT, &self->num_ports,
          "Number of ports to use starting at base-port (default 1)",
        "http-num-ports"}
    ,
    {"http-max-threads", 'm', 0, G_OPTION_ARG_INT, &self->max_threads,
          "Max number of allowed threads to process simultaneous requests. -1 "
          "means unlimited (default -1)",
        "http-max-threads"}
    ,
    {NULL}
  };
  GST_DEBUG_OBJECT (self, "HTTP init group callback ");
  *group = g_option_group_new ("gstd-http", ("HTTP Options"),
      ("Show HTTP Options"), NULL, NULL);

  g_option_group_add_entries (*group, http_args);
  return TRUE;
}

static GstdReturnCode
gstd_http_stop (GstdIpc * base)
{
  GstdHttp *self = GSTD_HTTP (base);
  GstdSession *session = base->session;

  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Entering SOCKET stop ");

      GST_INFO_OBJECT (session, "Closing SOCKET connection for %s",
          GSTD_OBJECT_NAME (session));

  return GSTD_EOK;
}
