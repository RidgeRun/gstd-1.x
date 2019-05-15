/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2019 Ridgerun, LLC (http://www.ridgerun.com)
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
#include "gstd_socket.h"
#include "gstd_parser.h"
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"


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
  GST_INFO_OBJECT (self, "Initializing gstd Socket");
  GstdIpc *base = GSTD_IPC (self);
  self->service = NULL;
  base->enabled = FALSE;
}

static void
gstd_socket_dispose (GObject * object)
{
  GstdSocket *self = GSTD_SOCKET (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd SOCKET");

  if (self->service) {
    self->service = NULL;
  }

  G_OBJECT_CLASS (gstd_socket_parent_class)->dispose (object);
}



static gboolean
gstd_socket_callback (GSocketService * service,
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

  while (TRUE) {
    read = g_input_stream_read (istream, message, size, NULL, NULL);

    /* Was connection closed? */
    if (read <= 0) {
      break;
    }
    message[read] = '\0';

    ret = gstd_parser_parse_cmd (session, message, &output); // in the parser

    /* Prepend the code to the output */
    description = gstd_return_code_to_string (ret);
    response =
      g_strdup_printf
      ("{\n  \"code\" : %d,\n  \"description\" : \"%s\",\n  \"response\" : %s\n}",
       ret, description, output ? output : "null");
    g_free (output);
    output = NULL;

    read = g_output_stream_write (ostream, response, strlen (response) + 1, NULL, NULL);
    if (read < 0) {
      break;
    }
    g_free (response);
  }

  g_free (message);

  return TRUE;
}

GstdReturnCode
gstd_socket_start (GstdIpc * base, GstdSession * session)
{
  guint debug_color;
  GstdSocket *self = GSTD_SOCKET (base);
  GSocketService **service;

  if (!base->enabled) {
    GST_DEBUG_OBJECT (self, "SOCKET not enabled, skipping");
    goto out;
  }

  GST_DEBUG_OBJECT (self, "Starting SOCKET");

  if (!gstd_socket_debug) {
    /* Initialize debug category with nice colors */
    debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
    GST_DEBUG_CATEGORY_INIT (gstd_socket_debug, "gstdsocket", debug_color,
        "Gstd SOCKET category");
  }

  // Close any existing connection
  gstd_socket_stop (base);

  service = &self->service;

 GstdReturnCode ret;
 ret = GSTD_SOCKET_GET_CLASS (self)->add_listener_address (self, service);

 if(ret != GSTD_EOK)
  return ret;

  /* listen to the 'incoming' signal */
  g_signal_connect (*service, "run", G_CALLBACK (gstd_socket_callback), session);

  /* start the socket service */
  g_socket_service_start (*service);

out:
  return GSTD_EOK;
}

GstdReturnCode
gstd_socket_stop (GstdIpc * base)
{
  GstdSocket *self = GSTD_SOCKET (base);
  GSocketService **service;
  GstdSession *session = base->session;

  g_return_val_if_fail (session, GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Entering SOCKET stop ");
  if (self->service) {
    service = &self->service;
    GSocketListener *listener = G_SOCKET_LISTENER (*service);
    if (*service) {
      GST_INFO_OBJECT (session, "Closing SOCKET connection for %s",
          GSTD_OBJECT_NAME (session));
      g_socket_listener_close (listener);
      g_socket_service_stop (*service);
      g_object_unref (*service);
      *service = NULL;
    }
  }
  return GSTD_EOK;
}
