/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "gstd_tcp.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_tcp_debug);
#define GST_CAT_DEFAULT gstd_tcp_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/* VTable */
static gboolean
gstd_tcp_callback  (GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data);

static gboolean
gstd_tcp_callback  (GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data) {

  GInputStream *istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  gchar message[1024];
  g_input_stream_read (istream,
		       message,
		       1,
		       NULL,
		       NULL);
  
  g_print("Message was: \"%s\"\n", message);
  return FALSE;
}

GstdReturnCode
gstd_tcp_start (GstdCore *core, GSocketService **service, guint16 port)
{
  guint debug_color;
  GError *error = NULL;

  if (!gstd_tcp_debug) {
    /* Initialize debug category with nice colors */
    debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
    GST_DEBUG_CATEGORY_INIT (gstd_tcp_debug, "gstdtcp", debug_color,
			     "Gstd TCP category");
  }

  // Close any existing connection
  gstd_tcp_stop (core, service);
  
  *service = g_socket_service_new ();

  g_socket_listener_add_inet_port (G_SOCKET_LISTENER(*service),
				   port, NULL/* G_OBJECT(core) */, &error);
  if (error)
    goto noconnection;

  /* listen to the 'incoming' signal */
  g_signal_connect (*service,
		    "incoming",
		    G_CALLBACK (gstd_tcp_callback),
		    NULL);
  
  /* start the socket service */
  g_socket_service_start (*service);
  
  return GSTD_EOK;
  
 noconnection:
  {
    GST_ERROR_OBJECT(core, "%s", error->message);
    g_error_free (error);
    return GSTD_NO_CONNECTION;
  }
    
}

GstdReturnCode
gstd_tcp_stop (GstdCore *core, GSocketService **service)
{
  GSocketListener *listener = G_SOCKET_LISTENER(*service);

  g_return_val_if_fail(core, GSTD_NULL_ARGUMENT);
  
  if (*service) {
    GST_INFO_OBJECT(core, "Closing TCP connection for %s",
		    GSTD_OBJECT_NAME(core));
    g_socket_listener_close (listener);
    g_socket_service_stop (*service);
    g_object_unref (*service);
    *service = NULL;
  }
  
  return GSTD_EOK;
}
