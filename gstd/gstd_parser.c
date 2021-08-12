/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2020 Ridgerun, LLC (http://www.ridgerun.com)
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

#include "gstd_event_handler.h"
#include "gstd_parser.h"
#include "gstd_session.h"

#define check_argument(arg, code) \
    if (NULL == (arg)) return (code)

/**
 * Prototypes for the functions
 */
static GstdReturnCode gstd_parser_create (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_parser_read (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** reponse);
static GstdReturnCode gstd_parser_update (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_parser_delete (GstdSession * session,
    GstdObject * obj, gchar * args, gchar ** response);
static GstdReturnCode gstd_parser_parse_raw_cmd (GstdSession * session,
    gchar * action, gchar * args, gchar ** response);
static GstdReturnCode gstd_parser_pipeline_create (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_delete (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_play (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_pause (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_stop (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_graph (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_pipeline_verbose (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_element_set (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_element_get (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_list_pipelines (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_list_elements (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_list_properties (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_list_signals (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_bus_read (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_bus_filter (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_bus_timeout (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_event_eos (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_event_seek (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_event_flush_start (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_event_flush_stop (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_signal_connect (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_signal_timeout (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_signal_disconnect (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_debug_enable (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_debug_threshold (GstdSession *, gchar *,
    gchar *, gchar **);
static GstdReturnCode gstd_parser_debug_color (GstdSession *, gchar *, gchar *,
    gchar **);
static GstdReturnCode gstd_parser_debug_reset (GstdSession *, gchar *, gchar *,
    gchar **);

typedef GstdReturnCode GstdFunc (GstdSession *, gchar *, gchar *, gchar **);
typedef struct _GstdCmd
{
  const gchar *cmd;
  GstdFunc *callback;
} GstdCmd;

static GstdCmd cmds[] = {
  {"create", gstd_parser_parse_raw_cmd},
  {"read", gstd_parser_parse_raw_cmd},
  {"update", gstd_parser_parse_raw_cmd},
  {"delete", gstd_parser_parse_raw_cmd},

  {"pipeline_create", gstd_parser_pipeline_create},
  {"pipeline_delete", gstd_parser_pipeline_delete},
  {"pipeline_play", gstd_parser_pipeline_play},
  {"pipeline_pause", gstd_parser_pipeline_pause},
  {"pipeline_stop", gstd_parser_pipeline_stop},
  {"pipeline_get_graph", gstd_parser_pipeline_graph},
  {"pipeline_verbose", gstd_parser_pipeline_verbose},

  {"element_set", gstd_parser_element_set},
  {"element_get", gstd_parser_element_get},

  {"list_pipelines", gstd_parser_list_pipelines},
  {"list_elements", gstd_parser_list_elements},
  {"list_properties", gstd_parser_list_properties},
  {"list_signals", gstd_parser_list_signals},

  {"bus_read", gstd_parser_bus_read},
  {"bus_filter", gstd_parser_bus_filter},
  {"bus_timeout", gstd_parser_bus_timeout},

  {"event_eos", gstd_parser_event_eos},
  {"event_seek", gstd_parser_event_seek},
  {"event_flush_start", gstd_parser_event_flush_start},
  {"event_flush_stop", gstd_parser_event_flush_stop},

  {"signal_connect", gstd_parser_signal_connect},
  {"signal_timeout", gstd_parser_signal_timeout},
  {"signal_disconnect", gstd_parser_signal_disconnect},

  {"debug_enable", gstd_parser_debug_enable},
  {"debug_threshold", gstd_parser_debug_threshold},
  {"debug_color", gstd_parser_debug_color},
  {"debug_reset", gstd_parser_debug_reset},

  {NULL}
};

static GstdReturnCode
gstd_parser_parse_raw_cmd (GstdSession * session, gchar * action, gchar * args,
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
    uri = (gchar *) "/";

  ret = gstd_get_by_uri (session, uri, &node);
  if (ret || NULL == node) {
    goto out;
  }

  if (!g_ascii_strcasecmp ("CREATE", action)) {
    ret = gstd_parser_create (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("READ", action)) {
    ret = gstd_parser_read (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("UPDATE", action)) {
    ret = gstd_parser_update (session, node, rest, response);
  } else if (!g_ascii_strcasecmp ("DELETE", action)) {
    ret = gstd_parser_delete (session, node, rest, response);
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

GstdReturnCode
gstd_parser_parse_cmd (GstdSession * session, const gchar * cmd,
    gchar ** response)
{
  gchar **tokens;
  gchar *action, *args;
  GstdCmd *cb;
  GstdReturnCode ret = GSTD_BAD_COMMAND;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (cmd, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*response);

  tokens = g_strsplit (cmd, " ", 2);
  action = tokens[0];
  args = tokens[1];

  cb = cmds;
  while (cb->cmd) {
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
gstd_parser_create (GstdSession * session, GstdObject * obj, gchar * args,
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
gstd_parser_read (GstdSession * session, GstdObject * obj, gchar * args,
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
gstd_parser_update (GstdSession * session, GstdObject * obj, gchar * args,
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
gstd_parser_delete (GstdSession * session, GstdObject * obj, gchar * args,
    gchar ** response)
{
  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  *response = NULL;

  return gstd_object_delete (obj, args);
}


static GstdReturnCode
gstd_parser_pipeline_create (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines %s", args ? args : "");

  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_delete (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines %s", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "delete", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_play (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state playing", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_pause (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state paused", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_stop (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/state null", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_graph (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/graph", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_pipeline_verbose (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret = GSTD_BAD_COMMAND;

#if GST_VERSION_MINOR >= 10
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);



  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/verbose %s", tokens[0], tokens[1]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

#else
  GST_ERROR_OBJECT (session, "GST v.%d.%d does not support deep notify",
      GST_VERSION_MAJOR, GST_VERSION_MINOR);
#endif

  return ret;
}

static GstdReturnCode
gstd_parser_element_set (GstdSession * session, gchar * action, gchar * args,
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
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_element_get (GstdSession * session, gchar * action, gchar * args,
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
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_list_pipelines (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines");
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_list_elements (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/elements/", args);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);
  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_list_properties (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 2);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);

  uri =
      g_strdup_printf ("/pipelines/%s/elements/%s/properties", tokens[0],
      tokens[1]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_list_signals (GstdSession * session, gchar * action, gchar * args,
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

  uri =
      g_strdup_printf ("/pipelines/%s/elements/%s/signals", tokens[0],
      tokens[1]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_bus_read (GstdSession * session, gchar * action,
    gchar * pipeline, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/bus/message", pipeline);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_bus_filter (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
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
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_bus_timeout (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
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
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_event_eos (GstdSession * session, gchar * action, gchar * pipeline,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/event eos", pipeline);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_event_seek (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
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
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "create", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_event_flush_start (GstdSession * session, gchar * action,
    gchar * pipeline, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipeline, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/event flush_start", pipeline);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "create", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_event_flush_stop (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
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

  uri =
      g_strdup_printf ("/pipelines/%s/event flush_stop %s", tokens[0],
      tokens[1]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "create", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}

static GstdReturnCode
gstd_parser_debug_enable (GstdSession * session, gchar * action,
    gchar * enabled, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (enabled, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/enable %s", enabled);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_debug_threshold (GstdSession * session, gchar * action,
    gchar * threshold, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (threshold, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/threshold %s", threshold);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);

  return ret;
}

static GstdReturnCode
gstd_parser_debug_color (GstdSession * session, gchar * action, gchar * colored,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (colored, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/color %s", colored);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);

  return ret;
}


static GstdReturnCode
gstd_parser_debug_reset (GstdSession * session, gchar * action, gchar * reset,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  check_argument (reset, GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/debug/reset %s", reset);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);

  return ret;
}


static GstdReturnCode
gstd_parser_signal_connect (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 3);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);
  check_argument (tokens[2], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/signals/%s/callback",
      tokens[0], tokens[1], tokens[2]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}


static GstdReturnCode
gstd_parser_signal_disconnect (GstdSession * session, gchar * action,
    gchar * args, gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 3);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);
  check_argument (tokens[2], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/signals/%s/disconnect",
      tokens[0], tokens[1], tokens[2]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "read", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}



static GstdReturnCode
gstd_parser_signal_timeout (GstdSession * session, gchar * action, gchar * args,
    gchar ** response)
{
  GstdReturnCode ret;
  gchar *uri;
  gchar **tokens;

  g_return_val_if_fail (GSTD_IS_SESSION (session), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (args, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (response, GSTD_NULL_ARGUMENT);

  tokens = g_strsplit (args, " ", 4);
  check_argument (tokens[0], GSTD_BAD_COMMAND);
  check_argument (tokens[1], GSTD_BAD_COMMAND);
  check_argument (tokens[2], GSTD_BAD_COMMAND);
  check_argument (tokens[3], GSTD_BAD_COMMAND);

  uri = g_strdup_printf ("/pipelines/%s/elements/%s/signals/%s/timeout %s",
      tokens[0], tokens[1], tokens[2], tokens[3]);
  ret = gstd_parser_parse_raw_cmd (session, (gchar *) "update", uri, response);

  g_free (uri);
  g_strfreev (tokens);

  return ret;
}
