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
  GstdSocket parent;
  guint base_port;
  gchar *address;
  guint num_ports;
  gint max_threads;
  GSocketService *service;
};

struct _GstdHttpClass
{
  GstdSocketClass parent_class;
};

G_DEFINE_TYPE (GstdHttp, gstd_http, GSTD_TYPE_SOCKET);

/* VTable */

static void gstd_http_dispose (GObject *);
static GstdReturnCode gstd_http_create_socket_service (GstdSocket * base,
    GSocketService ** service);
static gboolean gstd_http_init_get_option_group (GstdIpc * base, GOptionGroup ** group);
/*static gboolean gstd_http_add_listeners(GSocketService *service, gchar * address, gint port, GError ** error);
static gboolean gstd_tcp_add_listeners(GSocketService *service, gchar * address, gint port, GError ** error);*/
static void
gstd_http_class_init (GstdHttpClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  GstdSocketClass* socket_class = GSTD_SOCKET_CLASS (klass);
  guint debug_color;
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_http_init_get_option_group);
  socket_class->create_socket_service =
      GST_DEBUG_FUNCPTR (gstd_http_create_socket_service);
  object_class->dispose = gstd_http_dispose;

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
do_post (SoupServer *server, SoupMessage *msg)
{
  soup_message_set_status (msg, SOUP_STATUS_OK);
  return;
}

static void
do_get (SoupServer *server, SoupMessage *msg)
{
  
  soup_message_set_status (msg, SOUP_STATUS_OK);
  soup_message_set_response (msg, "application/json",SOUP_MEMORY_STATIC, "{\"name\":\"John\", \"age\":31, \"city\":\"New York\"}",
              sizeof("{\"name\":\"Lenin\", \"age\":24, \"city\":\"PZ\"}")-1);
  return;
}

static void
server_callback (SoupServer *server, SoupMessage *msg,
		 const char *path, GHashTable *query,
		 SoupClientContext *context, gpointer data)
{
	SoupMessageHeadersIter iter;
	const char *name, *value;
  SoupURI *address;
  soup_message_set_flags (msg,SOUP_MESSAGE_NEW_CONNECTION);
	g_print ("%s %s HTTP/1.%d\n", msg->method, path,
		 soup_message_get_http_version (msg));
	soup_message_headers_iter_init (&iter, msg->request_headers);
	while (soup_message_headers_iter_next (&iter, &name, &value))
    g_print ("%s: %s\n", name, value);
    g_print("**%d",soup_message_is_keepalive (msg));
	if (msg->request_body->length)
		g_print ("%s\n", msg->request_body->data);


	if (msg->method == SOUP_METHOD_GET || msg->method == SOUP_METHOD_HEAD){
    soup_message_headers_append (msg->response_headers,"Access-Control-Allow-Origin", "*"); 
		do_get (server, msg);
    
    address = soup_message_get_uri(msg);
    g_print("ADDRESS: %s\n",soup_uri_get_path(address));
    
  }

	else{
    do_post (server, msg);
    
  }
		//soup_message_set_status (msg, SOUP_STATUS_NOT_IMPLEMENTED);

	g_print ("  -> %d %s\n\n", msg->status_code, msg->reason_phrase); 
}

GstdReturnCode
gstd_http_create_socket_service (GstdSocket * base, GSocketService ** service)
{
  GError *error = NULL;
  GstdHttp *self = GSTD_HTTP (base);
  guint16 port = self->base_port;
  SoupServer *server;
	
  /*GSList *uris, *u;
	char *str;
  GSocketAddress * sa;
  gchar *address = self->address;
  guint i;*/

  GST_DEBUG_OBJECT (self, "Getting HTTP Socket address");
  server = soup_server_new(SOUP_SERVER_SERVER_HEADER,"HTTP-Server",NULL);

  *service = g_threaded_socket_service_new (self->max_threads);
  
  //sa = g_inet_socket_address_new_from_string(address, port);

    

  soup_server_listen_all (server, port+1, 0, &error);
  if (error){
    goto noconnection;
  }
  soup_server_add_handler (server, NULL, server_callback, NULL, NULL);
	g_print ("\nWaiting for requests...\n");
  

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

