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

#include <string.h>

#include "gstd_parser.h"

#include "gstd_socket.h"

/* Gstd SOCKET debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_socket_debug);
#define GST_CAT_DEFAULT gstd_socket_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdSocket, gstd_socket, GSTD_TYPE_IPC);

/* VTable */

static gboolean
gstd_socket_callback (GSocketService * service,
    GSocketConnection * connection,
    GObject * source_object, gpointer user_data);
static void gstd_socket_dispose (GObject *);
static GstdReturnCode gstd_socket_start (GstdIpc * base, GstdSession * session);
static GstdReturnCode gstd_socket_stop (GstdIpc * base);

static void
gstd_socket_class_init (GstdSocketClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  guint debug_color;
  gstdipc_class->start = GST_DEBUG_FUNCPTR (gstd_socket_start);
  gstdipc_class->stop = GST_DEBUG_FUNCPTR (gstd_socket_stop);
  object_class->dispose = gstd_socket_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_socket_debug, "gstdsocket", debug_color,
      "Gstd SOCKET category");
}

static void
gstd_socket_init (GstdSocket * self)
{
  GstdIpc *base = GSTD_IPC (self);
  GST_INFO_OBJECT (self, "Initializing gstd Socket");
  self->service = NULL;
  base->enabled = FALSE;
}

static void
gstd_socket_dispose (GObject * object)
{
  GST_INFO_OBJECT (object, "Deinitializing gstd SOCKET");

  G_OBJECT_CLASS (gstd_socket_parent_class)->dispose (object);
}



static gboolean
gstd_socket_callback (GSocketService * service,
    GSocketConnection * connection, GObject * source_object, gpointer user_data)
{

  GstdSession *session;
  GInputStream *istream;
  GOutputStream *ostream;
  gint read;
  const guint size = 1024 * 1024;
  gchar *output = NULL;
  gchar *response;
  gchar *message;
  GstdReturnCode ret;
  const gchar *description = NULL;

  g_return_val_if_fail (service, FALSE);
  g_return_val_if_fail (connection, FALSE);
  g_return_val_if_fail (user_data, FALSE);

  session = GSTD_SESSION (user_data);
  g_return_val_if_fail (session, FALSE);

  istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  message = g_malloc (size);

  while (TRUE) {
    read = g_input_stream_read (istream, message, size, NULL, NULL);

    /* Was connection closed? */
    if (read <= 0) {
      break;
    }
    message[read] = '\0';

    ret = gstd_parser_parse_cmd (session, message, &output);    // in the parser

    /* Prepend the code to the output */
    description = gstd_return_code_to_string (ret);
    response =
        g_strdup_printf
        ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
        ret, description, output ? output : "null");
    g_free (output);
    output = NULL;

    read =
        g_output_stream_write (ostream, response, strlen (response) + 1, NULL,
        NULL);
    if (read < 0) {
      break;
    }
    g_free (response);
  }

  g_free (message);

  return TRUE;
}

static GstdReturnCode
gstd_socket_start (GstdIpc * base, GstdSession * session)
{
  GstdSocket *self = GSTD_SOCKET (base);
  GSocketService *service;
  GstdReturnCode ret;

  GST_DEBUG_OBJECT (self, "Starting SOCKET");

  /* Close any existing connection */
  gstd_socket_stop (base);

  service = self->service;

  ret = GSTD_SOCKET_GET_CLASS (self)->create_socket_service (self, &service);

  if (ret != GSTD_EOK)
    return ret;

  /* listen to the 'incoming' signal */
  g_signal_connect (service, "run", G_CALLBACK (gstd_socket_callback), session);

  /* start the socket service */
  g_socket_service_start (service);

  return GSTD_EOK;
}

static GstdReturnCode
gstd_socket_stop (GstdIpc * base)
{
  GstdSocket *self = GSTD_SOCKET (base);
  GSocketService *service;
  GstdSession *session = base->session;
  GSocketListener *listener;

  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Entering SOCKET stop ");
  if (self->service) {
    service = self->service;
    listener = G_SOCKET_LISTENER (service);
    if (service) {
      GST_INFO_OBJECT (session, "Closing SOCKET connection for %s",
          GSTD_OBJECT_NAME (session));
      g_socket_listener_close (listener);
      g_socket_service_stop (service);
      g_object_unref (service);
      service = NULL;
    }
  }
  return GSTD_EOK;
}
