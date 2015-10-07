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
#include "gstd_element.h"
#include <stdio.h>
#include <ctype.h>

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
static GstdReturnCode
gstd_tcp_parse_cmd (GstdCore *core, const gchar *cmd, gchar **response);
static GstdReturnCode
gstd_tcp_create (GstdCore *core, GstdObject *obj, gchar *args, gchar **response);
static GstdReturnCode
gstd_tcp_read (GstdCore *core, GstdObject *obj, gchar *args, gchar **reponse);
static GstdReturnCode
gstd_tcp_update_by_type (GstdCore *core, GstdObject *obj, gchar *args);
  
static gboolean
gstd_tcp_callback  (GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data) {

  GstdCore *core = GSTD_CORE(user_data);
  GInputStream *istream;
  GOutputStream *ostream;
  gint read;
  const guint size = 1024*1024;
  gchar *output = NULL;
  gchar *response;
  gchar message[size];
  GstdReturnCode ret;

  g_return_val_if_fail (core, TRUE);

  istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  read = g_input_stream_read (istream,
			      message,
			      size,
			      NULL,
			      NULL);
  message[read] = '\0';

  ret = gstd_tcp_parse_cmd (core, message, &output);
  
  /* Prepend the code to the output */
  response = g_strdup_printf("{\n  code : %d\n  response : %s\n}", ret, output);
  g_free(output);
  
  g_output_stream_write (ostream,
  			 response,
  			 size,
  			 NULL,
  			 NULL);
  g_free(response);
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

static gboolean
gstd_tcp_is_num (const gchar *str)
{
  for (;*str != '\0'; str++){
    if (!g_ascii_isdigit(*str))
      return FALSE;
  }
  return TRUE;
}

static GstdReturnCode
gstd_tcp_create (GstdCore *core, GstdObject *obj, gchar *args, gchar **response)
{
  gchar **tokens;
  gchar *name;
  gchar *description;
  GstdObject *new;
  GstdReturnCode ret;
  
  g_return_val_if_fail (GSTD_IS_CORE(core), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT(obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  // This may mean a potential leak
  g_warn_if_fail (!*response);

  GST_FIXME_OBJECT(core, "Currently hardcoded to create pipelines, we must be "
		    "generic enough to create any type of object");

  // Tokens has the form {'name', <name>, 'description', <description>}
  tokens = g_strsplit(args, " ", 4);
  name = tokens[1];
  description = tokens[3];

  if (!name || name[0] == '\0')
    goto noname;

  if (!description || description[0] == '\0')
    goto nodescription;

  ret = gstd_object_create (obj, "name", name, "description", description, NULL);
  if (ret)
    goto noobject;

  gstd_object_read (obj, name, &new, NULL);
  gstd_object_to_string(new, response);
  g_object_unref(new);

  return ret;
  
 noname:
  {
    GST_ERROR_OBJECT(core, "Missing name for the new pipeline");
    g_strfreev(tokens);
    return GSTD_NULL_ARGUMENT;
  }
 nodescription:
  {
    GST_ERROR_OBJECT(core, "Missing description for pipeline \"%s\"", name);
    g_strfreev(tokens);
    return GSTD_NULL_ARGUMENT;
  }
 noobject:
  {
    g_strfreev(tokens);
    return ret;
  }
}

static GstdReturnCode
gstd_tcp_read (GstdCore *core, GstdObject *obj, gchar *args, gchar **response)
{
  gchar **tokens;
  GParamSpec *pspec;
  GObject *properties;
  GValue value = G_VALUE_INIT;
  gchar *svalue;
  
  g_return_val_if_fail (GSTD_IS_CORE(core), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT(obj), GSTD_NULL_ARGUMENT);

  // This may mean a potential leak
  g_warn_if_fail (!*response);
  
  // Print the raw object
  if (!args)
    return gstd_object_to_string (obj, response);

  tokens = g_strsplit (args, " ", -1);
  
  // Print the property
  /* If its a GstdElement element we need to parse the pspec from
     the internal element */
  if (GSTD_IS_ELEMENT(obj))
    gstd_object_read(obj, "gstelement", &properties, NULL);
  else
    properties = G_OBJECT(obj);
  
  pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(properties), tokens[0]);
  if (!pspec)
      goto noprop;

  /* Automagical type value serialization */
  g_value_init (&value, pspec->value_type);
  g_object_get_property(G_OBJECT(properties), tokens[0], &value);
  svalue = g_strdup_value_contents(&value);
  g_value_unset(&value);

  *response = g_strdup_printf ("{\n    %s : %s\n  }", tokens[0], svalue);
  g_free (svalue);
  
  return GSTD_EOK;

 noprop:
  {
    GST_ERROR_OBJECT(core, "Unexisting property \"%s\" in %s",
		     tokens[0], GSTD_OBJECT_NAME(obj));
    return GSTD_BAD_COMMAND;
  }
}

static GstdReturnCode
gstd_tcp_update_by_type (GstdCore *core, GstdObject *obj, gchar *args)
{
  gchar **tokens;
  gchar **property;
  gchar *prop;
  gchar *svalue;
  GParamSpec *pspec;
  GObject *properties;

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

    /* If its a GstdElement element we need to parse the pspec from
       the internal element */
    if (GSTD_IS_ELEMENT(obj))
      gstd_object_read(obj, "gstelement", &properties, NULL);
    else
      properties = G_OBJECT(obj);
    
    pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(properties), prop);
    if (!pspec)
      goto noprop;
    
    if (G_TYPE_CHAR == pspec->value_type ||
	G_TYPE_UCHAR == pspec->value_type ||
	G_TYPE_STRING == pspec->value_type) {
      return gstd_object_update (obj, prop, svalue, NULL);
    }
    
    if (G_TYPE_INT == pspec->value_type) {
      gint d;
      sscanf (svalue, "%d", &d);
      return gstd_object_update (obj, prop, d, NULL);
    }
    
    if (G_TYPE_UINT == pspec->value_type) {
      guint u;
      sscanf (svalue, "%u", &u);
      return gstd_object_update (obj, prop, u, NULL);
    }
    
    if (G_TYPE_FLOAT == pspec->value_type) {
      gfloat f;
      sscanf (svalue, "%f", &f);
      return gstd_object_update (obj, prop, f, NULL);
    }
    
    if (G_TYPE_DOUBLE == pspec->value_type) {
      gdouble lf;
      sscanf (svalue, "%lf", &lf);
      return gstd_object_update (obj, prop, lf, NULL);
    }

    if (G_TYPE_BOOLEAN == pspec->value_type) {
      gboolean b;
      if (!g_ascii_strcasecmp(svalue, "true"))
	b = TRUE;
      else if (!g_ascii_strcasecmp(svalue, "false"))
	b = FALSE;
      else
	goto badboolean;
      
      return gstd_object_update (obj, prop, b, NULL);
    }

    /* Complex types so  we can refer to the enum value as
       their names rather than ugly numbers */
    if (G_TYPE_IS_ENUM(pspec->value_type)) {
      GEnumClass *c = g_type_class_ref (pspec->value_type);
      GEnumValue *e;
      gint d;

      /* Try by name */
      e = g_enum_get_value_by_name (c, svalue);
      if (e)
	return gstd_object_update (obj, prop, e->value, NULL);

      /* Try by nick */
      e = g_enum_get_value_by_nick (c, svalue);
      if (e)
	return gstd_object_update (obj, prop, e->value, NULL);

      /* Try by integer */
      if(gstd_tcp_is_num(svalue)) {
	sscanf (svalue, "%d", &d);
	return gstd_object_update (obj, prop, d, NULL);
      }

      /* Unknown! */
      goto unknown;
    }

    /* Complex types so  we can refer to the flag value as
       their names rather than ugly numbers */
    if (G_TYPE_IS_FLAGS(pspec->value_type)) {
      GFlagsClass *c = g_type_class_ref (pspec->value_type);
      GFlagsValue *e;
      gint d;

      /* Try by name */
      e = g_flags_get_value_by_name (c, svalue);
      if (e)
	return gstd_object_update (obj, prop, e->value, NULL);

      /* Try by nick */
      e = g_flags_get_value_by_nick (c, svalue);
      if (e)
	return gstd_object_update (obj, prop, e->value, NULL);

      /* Try by integer */
      if(gstd_tcp_is_num(svalue)) {
	sscanf (svalue, "%d", &d);
	return gstd_object_update (obj, prop, d, NULL);
      }

      /* Unknown! */
      goto unknown;
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
 unknown:
  {
    GST_ERROR_OBJECT(core, "Invalid enum/flags value \"%s\"", svalue);
    g_strfreev(tokens);
    return GSTD_BAD_VALUE;
  }
 badboolean:
  {
    GST_ERROR_OBJECT(core, "Invalid boolean value \"%s\"", svalue);
    g_strfreev(tokens);
    return GSTD_BAD_VALUE;
  }
}

static GstdReturnCode
gstd_tcp_parse_cmd (GstdCore *core, const gchar *cmd, gchar **response)
{
  gchar **tokens;
  gchar *action, *uri, *args;
  GstdObject *node;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_CORE(core), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (cmd, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*response);

  tokens = g_strsplit (cmd, " ", 3);
  action = tokens[0];
  uri = tokens[1];
  args = tokens[2];

  if (gstd_get_by_uri (core, uri, &node))
    goto nonode;
  
  if (!g_ascii_strcasecmp("CREATE", action)) {
    ret = gstd_tcp_create(core, node, args, response);
  } else if (!g_ascii_strcasecmp("READ", action)) {
    ret = gstd_tcp_read(core, node, args, response);
  } else if (!g_ascii_strcasecmp("UPDATE", action)) {
    ret = gstd_tcp_update_by_type (core, node, args);
    gstd_tcp_read(core, node, args, response);
  } else if (!g_ascii_strcasecmp("DELETE", action)) {
    ret = gstd_object_delete (node, args);
  } else
    goto badcommand;
  
  return ret;

 nonode:
  {
    GST_ERROR_OBJECT(core, "Malformed URI \"%s\"", uri);
    g_strfreev (tokens);
    return GSTD_NO_RESOURCE;
  }
 badcommand:
  {
    GST_ERROR_OBJECT(core, "Unknown command \"%s\"", action);
    g_strfreev (tokens);
    g_object_unref (node);
    return GSTD_BAD_COMMAND;
  }
}
