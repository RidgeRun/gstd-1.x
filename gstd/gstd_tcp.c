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
#include <stdio.h>

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
static gchar*
gstd_tcp_parse_cmd (GstdCore *core, const gchar *cmd);

static gboolean
gstd_tcp_callback  (GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data) {

  GstdCore *core = GSTD_CORE(user_data);
  GInputStream *istream;
  GOutputStream *ostream;

  g_return_val_if_fail (core, TRUE);

  istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));
  
  gchar message[1024];
  g_input_stream_read (istream,
		       message,
		       1024,
		       NULL,
		       NULL);

  gstd_tcp_parse_cmd (core, message);
  g_output_stream_write (ostream,
			 message,
			 1024,
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
		    core);
  
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

static GstdReturnCode
gstd_tcp_update_by_type (GstdCore *core, GstdObject *obj, gchar *args)
{
  gchar **tokens;
  gchar **property;
  gchar *prop;
  gchar *svalue;
  GParamSpec *pspec;

  g_return_val_if_fail (GSTD_IS_CORE(core), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT(obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", -1);
  property = tokens;
  while (*property) {
    prop = *property++;
    svalue = *property++;
    
    if (!*svalue)
      goto novalue;
    
    pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj), prop);
    if (!pspec)
      goto noprop;
    
    if (G_TYPE_CHAR == pspec->value_type ||
	G_TYPE_UCHAR == pspec->value_type ||
	G_TYPE_STRING == pspec->value_type)
      {
	return gstd_object_update (obj, prop, svalue, NULL);
      }
    if (G_TYPE_INT == pspec->value_type)
      {
	gint d;
	sscanf (svalue, "%d", &d);
	return gstd_object_update (obj, prop, d, NULL);
      }
    if (G_TYPE_UINT == pspec->value_type)
      {
	guint u;
	sscanf (svalue, "%u", &u);
	return gstd_object_update (obj, prop, u, NULL);
      }
    if (G_TYPE_FLOAT == pspec->value_type)
      {
	gfloat f;
	sscanf (svalue, "%f", &f);
	return gstd_object_update (obj, prop, f, NULL);
      }
    if (G_TYPE_DOUBLE == pspec->value_type)
      {
	gdouble lf;
	sscanf (svalue, "%lf", &lf);
	return gstd_object_update (obj, prop, lf, NULL);
      }

    GST_ERROR_OBJECT(core, "Unable to handle \"%s\" types",
		     g_type_name(pspec->value_type));
    g_strfreev(tokens);
    return GSTD_BAD_COMMAND;
  }

 novalue:
  {
    GST_ERROR_OBJECT(core, "Missing value for property");
    g_strfreev(tokens);
    return GSTD_BAD_COMMAND;
  }
 noprop:
  {
    GST_ERROR_OBJECT(core, "Unexisting property \"%s\" in %s",
		     prop, GSTD_OBJECT_NAME(obj));
    g_strfreev(tokens);
    return GSTD_BAD_COMMAND;
  }
}

static gchar*
gstd_tcp_parse_cmd (GstdCore *core, const gchar *cmd)
{
  gchar *out = NULL;
  gchar **tokens;
  gchar *action, *uri, *args;
  GstdObject *node;

  g_return_val_if_fail (GSTD_IS_CORE(core), NULL);
  g_return_val_if_fail (cmd, NULL);

  tokens = g_strsplit (cmd, " ", 3);
  action = tokens[0];
  uri = tokens[1];
  args = tokens[2];

  if (gstd_get_by_uri (core, uri, &node))
    goto nonode;
  
  if (!g_ascii_strcasecmp("CREATE", action)) {
    g_print ("CREATE - %s - %s\n", GSTD_OBJECT_NAME(node), args); 
  } else if (!g_ascii_strcasecmp("READ", action)) {
    gchar *str = NULL;
    gstd_object_to_string(node, &str);
    g_print ("%s\n", str);
    g_free (str);
  } else if (!g_ascii_strcasecmp("UPDATE", action)) {
    gstd_tcp_update_by_type (core, node, args);
  } else if (!g_ascii_strcasecmp("DELETE", action)) {
    if (!args)
      goto noargtodelete;
    gstd_object_delete (node, args);
  } else
    goto badcommand;
  
  return out;

 nonode:
  {
    GST_ERROR_OBJECT(core, "Malformed URI \"%s\"", uri);
    g_strfreev (tokens);
    return NULL;
  }
 badcommand:
  {
    GST_ERROR_OBJECT(core, "Unknown command \"%s\"", action);
    g_strfreev (tokens);
    g_object_unref (node);
    return NULL;
  }
 noargtodelete:
  {
    GST_ERROR_OBJECT(core, "Missing name of resource to delete");
    g_strfreev (tokens);
    g_object_unref (node);
    return NULL;
  }
}
