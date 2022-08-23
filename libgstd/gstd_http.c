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
#include <libsoup/soup.h>

#include "gstd_http.h"
#include "gstd_parser.h"

/* Gstd HTTP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_http_debug);
#define GST_CAT_DEFAULT gstd_http_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

typedef struct _GstdHttpRequest
{
  SoupServer *server;
  SoupMessage *msg;
  GstdSession *session;
  const char *path;
  GHashTable *query;
  GMutex *mutex;
} GstdHttpRequest;

struct _GstdHttp
{
  GstdIpc parent;
  guint port;
  gchar *address;
  gint max_threads;
  SoupServer *server;
  GstdSession *session;
  GThreadPool *pool;
  GMutex mutex;
};

struct _GstdHttpClass
{
  GstdIpcClass parent_class;
};

G_DEFINE_TYPE (GstdHttp, gstd_http, GSTD_TYPE_IPC);

/* VTable */

static void gstd_http_finalize (GObject *);
static GstdReturnCode gstd_http_start (GstdIpc * base, GstdSession * session);
static GstdReturnCode gstd_http_stop (GstdIpc * base);
static gboolean gstd_http_init_get_option_group (GstdIpc * base,
    GOptionGroup ** group);
static SoupStatus get_status_code (GstdReturnCode ret);
static GstdReturnCode do_get (SoupServer * server, SoupMessage * msg,
    char **output, const char *path, GstdSession * session);
static GstdReturnCode do_post (SoupServer * server, SoupMessage * msg,
    char *name, char *description, char **output, const char *path,
    GstdSession * session);
static GstdReturnCode do_put (SoupServer * server, SoupMessage * msg,
    char *name, char **output, const char *path, GstdSession * session);
static GstdReturnCode do_delete (SoupServer * server, SoupMessage * msg,
    char *name, char **output, const char *path, GstdSession * session);
static void do_request (gpointer data_request, gpointer eval);
static void server_callback (SoupServer * server, SoupMessage * msg,
    const char *path, GHashTable * query, SoupClientContext * context,
    gpointer data);

static void
gstd_http_class_init (GstdHttpClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  guint debug_color;

  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_http_init_get_option_group);
  gstdipc_class->start = GST_DEBUG_FUNCPTR (gstd_http_start);
  object_class->finalize = gstd_http_finalize;
  gstdipc_class->stop = GST_DEBUG_FUNCPTR (gstd_http_stop);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_http_debug, "gstdhttp", debug_color,
      "Gstd HTTP category");
}

static void
gstd_http_init (GstdHttp * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd Http");
  g_mutex_init (&self->mutex);
  self->port = GSTD_HTTP_DEFAULT_PORT;
  self->address = g_strdup (GSTD_HTTP_DEFAULT_ADDRESS);
  self->max_threads = GSTD_HTTP_DEFAULT_MAX_THREADS;
  self->server = NULL;
  self->session = NULL;
  self->pool = NULL;

}

static void
gstd_http_finalize (GObject * object)
{
  GstdHttp *self = GSTD_HTTP (object);
  GstdIpc *ipc = GSTD_IPC (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd HTTP");

  if (ipc->enabled) {
    gstd_http_stop (ipc);
  }

  g_mutex_clear (&self->mutex);

  if (self->address) {
    g_free (self->address);
    self->address = NULL;
  }

  if (self->pool) {
    g_thread_pool_free (self->pool, FALSE, TRUE);
    self->pool = NULL;
  }

  G_OBJECT_CLASS (gstd_http_parent_class)->finalize (object);
}

static SoupStatus
get_status_code (GstdReturnCode ret)
{
  SoupStatus status = SOUP_STATUS_OK;

  if (ret == GSTD_EOK) {
    status = SOUP_STATUS_OK;
  } else if (ret == GSTD_BAD_COMMAND || ret == GSTD_NO_RESOURCE) {
    status = SOUP_STATUS_NOT_FOUND;
  } else if (ret == GSTD_EXISTING_RESOURCE) {
    status = SOUP_STATUS_CONFLICT;
  } else if (ret == GSTD_BAD_VALUE) {
    status = SOUP_STATUS_NO_CONTENT;
  } else {
    status = SOUP_STATUS_BAD_REQUEST;
  }

  return status;
}

static GstdReturnCode
do_get (SoupServer * server, SoupMessage * msg, char **output, const char *path,
    GstdSession * session)
{
  gchar *message = NULL;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (server, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (output, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (path, GSTD_NULL_ARGUMENT);

  message = g_strdup_printf ("read %s", path);
  ret = gstd_parser_parse_cmd (session, message, output);
  g_free (message);
  message = NULL;

  return ret;
}

static GstdReturnCode
do_post (SoupServer * server, SoupMessage * msg, char *name,
    char *description, char **output, const char *path, GstdSession * session)
{
  gchar *message = NULL;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (server, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (path, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (output, GSTD_NULL_ARGUMENT);

  if (!name) {
    ret = GSTD_BAD_VALUE;
    GST_ERROR_OBJECT (session,
        "Wrong query param provided, \"name\" doesn't exist");
    goto out;
  }

  if (description) {
    message = g_strdup_printf ("create %s %s %s", path, name, description);
  } else {
    message = g_strdup_printf ("create %s %s", path, name);
  }

  ret = gstd_parser_parse_cmd (session, message, output);
  g_free (message);
  message = NULL;

out:
  return ret;
}

static GstdReturnCode
do_put (SoupServer * server, SoupMessage * msg, char *name, char **output,
    const char *path, GstdSession * session)
{
  gchar *message = NULL;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (server, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (output, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (path, GSTD_NULL_ARGUMENT);

  if (!name) {
    ret = GSTD_BAD_VALUE;
    GST_ERROR_OBJECT (session,
        "Wrong query param provided, \"name\" doesn't exist");
    goto out;
  }

  message = g_strdup_printf ("update %s %s", path, name);
  ret = gstd_parser_parse_cmd (session, message, output);
  g_free (message);
  message = NULL;

out:
  return ret;
}

static GstdReturnCode
do_delete (SoupServer * server, SoupMessage * msg, char *name,
    char **output, const char *path, GstdSession * session)
{
  gchar *message = NULL;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (server, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (msg, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (output, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (path, GSTD_NULL_ARGUMENT);

  if (!name) {
    ret = GSTD_BAD_VALUE;
    GST_ERROR_OBJECT (session,
        "Wrong query param provided, \"name\" doesn't exist");
    goto out;
  }

  message = g_strdup_printf ("delete %s %s", path, name);
  ret = gstd_parser_parse_cmd (session, message, output);
  g_free (message);
  message = NULL;

out:
  return ret;
}

static void
do_request (gpointer data_request, gpointer eval)
{
  gchar *response = NULL;
  gchar *name = NULL;
  gchar *description_pipe = NULL;
  GstdReturnCode ret = GSTD_BAD_COMMAND;
  gchar *output = NULL;
  const gchar *description = NULL;
  SoupStatus status = SOUP_STATUS_OK;
  SoupServer *server = NULL;
  SoupMessage *msg = NULL;
  GstdSession *session = NULL;
  const char *path = NULL;
  GHashTable *query = NULL;
  GstdHttpRequest *data_request_local = NULL;

  g_return_if_fail (data_request);

  data_request_local = (GstdHttpRequest *) data_request;
  g_mutex_lock (data_request_local->mutex);
  server = data_request_local->server;
  g_mutex_unlock (data_request_local->mutex);
  msg = data_request_local->msg;
  session = data_request_local->session;
  path = data_request_local->path;
  query = data_request_local->query;

  if (query != NULL) {
    name = g_hash_table_lookup (query, "name");
    description_pipe = g_hash_table_lookup (query, "description");
  }

  if (msg->method == SOUP_METHOD_GET) {
    ret = do_get (server, msg, &output, path, session);
  } else if (msg->method == SOUP_METHOD_POST) {
    ret = do_post (server, msg, name, description_pipe, &output, path, session);
  } else if (msg->method == SOUP_METHOD_PUT) {
    ret = do_put (server, msg, name, &output, path, session);
  } else if (msg->method == SOUP_METHOD_DELETE) {
    ret = do_delete (server, msg, name, &output, path, session);
  } else if (msg->method == SOUP_METHOD_OPTIONS) {
    ret = GSTD_EOK;
  }

  description = gstd_return_code_to_string (ret);
  response =
      g_strdup_printf
      ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
      ret, description, output ? output : "null");
  g_free (output);
  output = NULL;

  soup_message_set_response (msg, "application/json", SOUP_MEMORY_COPY,
      response, strlen (response));
  g_free (response);
  response = NULL;

  status = get_status_code (ret);
  soup_message_set_status (msg, status);
  g_mutex_lock (data_request_local->mutex);
  soup_server_unpause_message (server, msg);
  g_mutex_unlock (data_request_local->mutex);

  if (query != NULL) {
    g_hash_table_unref (query);
  }
  free (data_request);
  data_request = NULL;

  return;
}

static void
server_callback (SoupServer * server, SoupMessage * msg,
    const char *path, GHashTable * query,
    SoupClientContext * context, gpointer data)
{
  GstdSession *session = NULL;
  GstdHttp *self = NULL;
  GstdHttpRequest *data_request = NULL;

  g_return_if_fail (server);
  g_return_if_fail (msg);
  g_return_if_fail (data);

  self = GSTD_HTTP (data);
  session = self->session;

  data_request = (GstdHttpRequest *) malloc (sizeof (GstdHttpRequest));

  data_request->msg = msg;
  data_request->server = server;
  data_request->session = session;
  data_request->path = path;
  if (query) {
    data_request->query = g_hash_table_ref (query);
  } else {
    data_request->query = query;
  }
  data_request->mutex = &self->mutex;

  soup_message_headers_append (msg->response_headers,
      "Access-Control-Allow-Origin", "*");
  soup_message_headers_append (msg->response_headers,
      "Access-Control-Allow-Headers", "origin,range,content-type");
  soup_message_headers_append (msg->response_headers,
      "Access-Control-Allow-Methods", "PUT, GET, POST, DELETE");
  g_mutex_lock (&self->mutex);
  soup_server_pause_message (server, msg);
  g_mutex_unlock (&self->mutex);
  if (!g_thread_pool_push (self->pool, (gpointer) data_request, NULL)) {
    GST_ERROR_OBJECT (self->pool, "Thread pool push failed");
  }

}

static GstdReturnCode
gstd_http_start (GstdIpc * base, GstdSession * session)
{
  GError *error = NULL;
  GSocketAddress *sa = NULL;
  GstdHttp *self = NULL;
  guint16 port = 0;
  gchar *address = NULL;

  g_return_val_if_fail (base, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);

  self = GSTD_HTTP (base);
  port = self->port;
  address = self->address;

  self->session = session;
  gstd_http_stop (base);

  GST_DEBUG_OBJECT (self, "Initializing HTTP server");
  self->server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "Gstd-1.0", NULL);
  if (!self->server) {
    goto noconnection;
  }
  self->pool =
      g_thread_pool_new (do_request, NULL, self->max_threads, FALSE, &error);

  if (error) {
    goto noconnection;
  }

  sa = g_inet_socket_address_new_from_string (address, port);

  soup_server_listen (self->server, sa, 0, &error);

  if (error) {
    goto noconnection;
  }

  soup_server_add_handler (self->server, NULL, server_callback, self, NULL);

  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (self, "%s", error->message);
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    error = NULL;
    g_object_unref (self->server);
    self->server = NULL;
    return GSTD_NO_CONNECTION;
  }
}

static gboolean
gstd_http_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdHttp *self = GSTD_HTTP (base);

  GOptionEntry http_args[] = {
    {"enable-http-protocol", 't', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given HTTP ports ", NULL}
    ,
    {"http-address", 'a', 0, G_OPTION_ARG_STRING, &self->address,
          "Attach to the server through a given address (default 127.0.0.1)",
        "http-address"}
    ,
    {"http-port", 'p', 0, G_OPTION_ARG_INT, &self->port,
          "Attach to the server through a given port (default 5001)",
        "http-port"}
    ,
    {"http-max-threads", 'm', 0, G_OPTION_ARG_INT, &self->max_threads,
          "Max number of allowed threads to process simultaneous requests. -1 "
          "means unlimited (default -1)",
        "http-max-threads"}
    ,
    {NULL}
  };

  g_return_val_if_fail (base, FALSE);
  g_return_val_if_fail (group, FALSE);

  GST_DEBUG_OBJECT (self, "HTTP init group callback ");
  *group = g_option_group_new ("gstd-http", ("HTTP Options"),
      ("Show HTTP Options"), NULL, NULL);

  g_option_group_add_entries (*group, http_args);
  return TRUE;
}

static GstdReturnCode
gstd_http_stop (GstdIpc * base)
{
  GstdHttp *self = NULL;
  GstdSession *session = NULL;

  g_return_val_if_fail (base, GSTD_NULL_ARGUMENT);

  self = GSTD_HTTP (base);
  session = base->session;

  GST_INFO_OBJECT (session, "Closing HTTP server connection for %s",
      GSTD_OBJECT_NAME (session));

  if (self->server) {
    g_object_unref (self->server);
  }
  self->server = NULL;

  return GSTD_EOK;
}
