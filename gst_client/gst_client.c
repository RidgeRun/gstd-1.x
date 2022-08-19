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

#include <errno.h>
#include <editline/readline.h>
#include <gio/gio.h>
#include <gio/gunixsocketaddress.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <locale.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* cmdline defaults */
#define GSTD_CLIENT_DEFAULT_TCP_INET_ADDRESS "localhost"
#define GSTD_CLIENT_DEFAULT_UNIX_BASE_NAME "gstd_unix_socket"
#define GSTD_CLIENT_DEFAULT_TCP_PORT 5000
#define GSTD_CLIENT_DEFAULT_UNIX_PORT 0
#define GSTD_CLIENT_MAX_RESPONSE 10485760       /* 10*1024*1024 */
#define GSTD_CLIENT_DOMAIN "gst-client"

static GQuark quark;

typedef struct _GstdClientData GstdClientData;
typedef struct _GstdClientCmd GstdClientCmd;
typedef gint GstdClientCB (gchar *, gchar *, GstdClientData *);

struct _GstdClientCmd
{
  const gchar *name;
  GstdClientCB *func;
  const gchar *doc;
  const gchar *usage;
};

struct _GstdClientData
{
  gboolean quiet;
  gboolean use_unix;
  const gchar *prompt;
  guint tcp_port;
  guint unix_port;
  gchar *address;
  GSocketClient *client;
  GSocketConnection *con;
};

/* VTable */
static gint gstd_client_cmd_quit (gchar *, gchar *, GstdClientData *);
static gint gstd_client_cmd_warranty (gchar *, gchar *, GstdClientData *);
static gint gstd_client_cmd_help (gchar *, gchar *, GstdClientData *);
static gint gstd_client_cmd_socket (gchar *, gchar *, GstdClientData *);
static gint gstd_client_cmd_sh (gchar *, gchar *, GstdClientData *);
static gint gstd_client_cmd_source (gchar *, gchar *, GstdClientData *);
static gchar *gstd_client_completer (const gchar *, gint);
static gint gstd_client_execute (gchar *, GstdClientData *);
static void gstd_client_header (gboolean quiet);
static void gstd_client_process_error (GError * error);

/* Global variables */

/* Control the program flow */
static gboolean quit;

/* Unblock readline on interrupt */
static sigjmp_buf ctrlc_buf;

/* List of available commands */
static GstdClientCmd cmds[] = {
  {"warranty", gstd_client_cmd_warranty, "Prints Gstd warranty", "warranty"},
  {"quit", gstd_client_cmd_quit, "Exits Gstd client", "quit"},
  {"exit", gstd_client_cmd_quit, "Exits Gstd client", "exit"},
  {"help", gstd_client_cmd_help, "Prints help information", "help [command]"},
  {"create", gstd_client_cmd_socket, "Creates a resource at the given URI",
      "create <URI> [property value ...]"},
  {"read", gstd_client_cmd_socket, "Reads the resource at the given URI",
      "read <URI>"},
  {"update", gstd_client_cmd_socket, "Updates the resource at the given URI",
      "update <URI> [property value ...]"},
  {"delete", gstd_client_cmd_socket,
        "Deletes the resource held at the given URI with the given name",
      "delete <URI> <name>"},
  {"sh", gstd_client_cmd_sh, "Executes a shell command", "sh <command>"},
  {"source", gstd_client_cmd_source, "Sources a file with commands",
      "source <file>"},

  // High level commands
  {"pipeline_create", gstd_client_cmd_socket,
        "Creates a new pipeline based on the name and description",
      "pipeline_create <name> <description>"},
  {"pipeline_create_ref", gstd_client_cmd_socket,
        "Creates a new pipeline based on the name and description using refcount",
      "pipeline_create_ref <name> <description>"},
  {"pipeline_delete", gstd_client_cmd_socket,
        "Deletes the pipeline with the given name",
      "pipeline_delete <name>"},
  {"pipeline_delete_ref", gstd_client_cmd_socket,
        "Deletes the pipeline with the given name using refcount",
      "pipeline_delete_ref <name>"},
  {"pipeline_play", gstd_client_cmd_socket, "Sets the pipeline to playing",
      "pipeline_play <name>"},
  {"pipeline_play_ref", gstd_client_cmd_socket,
        "Sets the pipeline to playing using refcount",
      "pipeline_play_ref <name>"},
  {"pipeline_pause", gstd_client_cmd_socket, "Sets the pipeline to paused",
      "pipeline_pause <name>"},
  {"pipeline_stop", gstd_client_cmd_socket, "Sets the pipeline to null",
      "pipeline_stop <name>"},
  {"pipeline_stop_ref", gstd_client_cmd_socket,
        "Sets the pipeline to null using refcount",
      "pipeline_stop_ref <name>"},
  {"pipeline_get_graph", gstd_client_cmd_socket, "Gets pipeline graph",
      "pipeline_get_graph <name>"},
  {"pipeline_verbose", gstd_client_cmd_socket, "Updates pipeline verbose",
      "pipeline_verbose <name> <value>"},

  {"element_set", gstd_client_cmd_socket,
        "Sets a property in an element of a given pipeline",
      "element_set <pipe> <element> <property> <value>"},
  {"element_get", gstd_client_cmd_socket,
        "Queries a property in an element of a given pipeline",
      "element_get <pipe> <element> <property>"},

  {"list_pipelines", gstd_client_cmd_socket, "List the existing pipelines",
      "list_pipelines"},
  {"list_elements", gstd_client_cmd_socket,
        "List the elements in a given pipeline",
      "list_elements <pipe>"},
  {"list_properties", gstd_client_cmd_socket,
        "List the properties of an element in a given pipeline",
      "list_properties <pipe> <element>"},
  {"list_signals", gstd_client_cmd_socket,
        "List the signals of an element in a given pipeline",
      "list_signals <pipe> <element>"},

  {"bus_read", gstd_client_cmd_socket, "Read from the bus",
      "bus_read <pipe>"},
  {"bus_filter", gstd_client_cmd_socket,
        "Select the types of message to be read from the bus. Separate with "
        "a '+', i.e.: eos+warning+error",
      "bus_filter <pipe> <filter>"},
  {"bus_timeout", gstd_client_cmd_socket,
        "Apply a timeout for the bus polling. -1: forever, 0: return immediately, "
        "n: wait n nanoseconds",
      "bus_timeout <pipe> <timeout>"},

  {"event_eos", gstd_client_cmd_socket, "Send an end-of-stream event",
      "event_eos <pipe>"},
  {"event_seek", gstd_client_cmd_socket,
        "Perform a seek in the given pipeline",
      "event_seek <pipe> <rate=1.0> <format=3> <flags=1> <start-type=1> <start=0> <end-type=1> <end=-1>"},
  {"event_flush_start", gstd_client_cmd_socket,
        "Put the pipeline in flushing mode",
      "event_flush_start <pipe>"},
  {"event_flush_stop", gstd_client_cmd_socket,
        "Take the pipeline out from flushing mode",
      "event_flush_stop <pipe> <reset=true>"},

  {"signal_connect", gstd_client_cmd_socket, "Connect to signal and wait",
      "signal_connect <pipe> <element> <signal>"},
  {"signal_timeout", gstd_client_cmd_socket,
        "Apply a timeout for the signal waiting. -1: forever, 0: return immediately, "
        "n: wait n microseconds",
      "signal_timeout <pipe> <element> <signal> <timeout>"},
  {"signal_disconnect", gstd_client_cmd_socket, "Disconnect from signal",
      "signal_disconnect <pipe> <element> <signal>"},

  {"action_emit", gstd_client_cmd_socket, "Emit action",
      "action_emit <pipe> <element> <action>"},

  {"debug_enable", gstd_client_cmd_socket,
        "Enable/Disable GStreamer debug",
      "debug_enable <enable>"},
  {"debug_threshold", gstd_client_cmd_socket,
        "The debug filter to apply (as you would use with gst-launch)",
      "debug_threshold <threshold>"},
  {"debug_color", gstd_client_cmd_socket,
        "Enable/Disable colors in the debug logging",
      "debug_color <colors>"},
  {"debug_reset", gstd_client_cmd_socket,
        "Enable/Disable debug threshold reset",
      "debug_reset <reset>"},

  {NULL}
};

/* Capture the infamous CTRL-C */
static void
sig_handler (gint signo)
{
  if (signo == SIGINT) {
    g_print ("Signal received, quitting...\n");
    quit = TRUE;
    siglongjmp (ctrlc_buf, 1);
  }
}

/* Customizing readline */
static void
init_readline (void)
{
  /* Allow conditional parsing of the ~/.inputrc file */
  rl_readline_name = g_strdup ("Gstd");

  /* Custom command completion */
  rl_completion_entry_function = gstd_client_completer;
}

/* Called with every line entered by the user */
static gint
gstd_client_execute (gchar * line, GstdClientData * data)
{
  gchar **tokens;
  gchar *name;
  gchar *arg;
  GstdClientCmd *cmd;
  gint ret;

  g_return_val_if_fail (line, -1);
  g_return_val_if_fail (data, -1);

  tokens = g_strsplit (line, " ", 2);
  name = g_strstrip (tokens[0]);

  /* Prefer an empty string than NULL */
  if (tokens[1])
    arg = g_strstrip (tokens[1]);
  else
    arg = (gchar *) "";

  /* Find and execute the respective command */
  cmd = cmds;
  while (cmd->name) {
    if (!strcmp (cmd->name, name)) {
      ret = cmd->func (name, arg, data);
      g_strfreev (tokens);
      return ret;
    }
    cmd++;
  }
  g_printerr ("No such command `%s'\n", name);
  g_strfreev (tokens);

  return -1;
}

gint
main (gint argc, gchar * argv[])
{
  gchar *line;
  GError *error = NULL;
  GOptionContext *context;
  const gchar *prompt = "gstd> ";
  const gchar *history = ".gstc_history";
  const gchar *history_env = "GSTC_HISTORY";
  gchar *history_full = NULL;
  GstdClientData *data;

  /* Cmdline options */
  gboolean quiet;
  gboolean inter;
  gboolean launch;
  gboolean version;
  gboolean use_unix;
  gchar *file;
  guint tcp_port;
  guint unix_port;
  gchar *address;
  gchar **remaining;

  GOptionEntry entries[] = {
    {"quiet", 'q', 0, G_OPTION_ARG_NONE, &quiet, "Dont't print startup headers",
        NULL}
    ,
    {"file", 'f', 0, G_OPTION_ARG_FILENAME, &file,
        "Execute the commands in file", "script"}
    ,
    {"interactive", 'i', 0, G_OPTION_ARG_NONE, &inter,
        "Enter interactive mode after executing cmdline", NULL}
    ,
    {"launch", 'l', 0, G_OPTION_ARG_NONE, &launch,
        "Emulate gst-launch, often combined with -i", NULL}
    ,
    {"tcp-port", 'p', 0, G_OPTION_ARG_INT, &tcp_port,
          "Attach to the server through the given port (default 5000)",
        "tcp-port"}
    ,
    {"tcp-address", 'a', 0, G_OPTION_ARG_STRING, &address,
        "The IP address of the server (defaults to "
          GSTD_CLIENT_DEFAULT_TCP_INET_ADDRESS ")", "address"}
    ,
    {"unix", 'u', 0, G_OPTION_ARG_NONE, &use_unix,
        "Use unix socket", NULL}
    ,
    {"unix-base-path", 'b', 0, G_OPTION_ARG_STRING, &address,
        "The server unix path (defaults to "
          GSTD_CLIENT_DEFAULT_UNIX_BASE_NAME
          "), use 'unix-port' to specify the port number", "path"}
    ,
    {"unix-port", 'e', 0, G_OPTION_ARG_INT, &unix_port,
          "Attach to the server through the given port (default 0)",
        "unix-port"}
    ,
    {"version", 'v', 0, G_OPTION_ARG_NONE, &version,
        "Print current gstd-client version", NULL}
    ,
    {G_OPTION_REMAINING, '0', 0, G_OPTION_ARG_STRING_ARRAY, &remaining,
        "commands", NULL}
    ,
    {NULL}
  };
  quark = g_quark_from_static_string (GSTD_CLIENT_DOMAIN);

  /* Internationalization */
  setlocale (LC_ALL, "");

  // Initialize default
  remaining = NULL;
  file = NULL;
  quit = FALSE;
  version = FALSE;
  inter = FALSE;
  tcp_port = GSTD_CLIENT_DEFAULT_TCP_PORT;
  unix_port = GSTD_CLIENT_DEFAULT_UNIX_PORT;
  address = NULL;
  quiet = FALSE;
  use_unix = FALSE;

  //Did the user pass in a custom history?
  if (g_getenv (history_env)) {
    history_full = g_strdup (g_getenv (history_env));
  } else {
    // Read home location from environment
    history_full = g_strdup_printf ("%s/%s", g_getenv ("HOME"), history);
  }

  context = g_option_context_new ("[COMMANDS...] - gst-launch under steroids");
  g_option_context_add_main_entries (context, entries, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("%s\n", error->message);
    return EXIT_FAILURE;
  }
  g_option_context_free (context);

  if (!address) {
    if (use_unix) {
      address =
          g_strdup_printf ("%s/%s", GSTD_RUN_STATE_DIR,
          GSTD_CLIENT_DEFAULT_UNIX_BASE_NAME);
    } else {
      address = g_strdup (GSTD_CLIENT_DEFAULT_TCP_INET_ADDRESS);
    }
  }
  // Enter interactive only if no commands nor file has been set, or if the
  // user explicitely asked for it
  inter = inter || (!file && !remaining);
  quiet = quiet || file || remaining;

  if (version) {
    g_print ("" PACKAGE_STRING "\n");
    g_print ("Copyright (c) 2015 RidgeRun Engineering\n");
    return EXIT_SUCCESS;
  }

  init_readline ();
  read_history (history_full);

  data = (GstdClientData *) g_malloc (sizeof (GstdClientData));
  data->quiet = quiet;
  data->prompt = prompt;
  data->tcp_port = tcp_port;
  data->unix_port = unix_port;
  data->address = address;
  data->use_unix = use_unix;
  data->client = g_socket_client_new ();
  data->con = NULL;

  /* Jump here in case of an interrupt, so we can exit */
  while (sigsetjmp (ctrlc_buf, 1) != 0);
  signal (SIGINT, sig_handler);

  if (file && !quit) {
    gstd_client_cmd_source ((gchar *) "source", file, data);
    g_free (file);
  }

  if (remaining && !quit) {
    line = g_strjoinv (" ", remaining);
    gstd_client_execute (line, data);
    g_free (line);
    g_strfreev (remaining);
  }

  if (!inter)
    return EXIT_SUCCESS;

  gstd_client_header (quiet);

  /* New interrupt checkpoint */
  while (sigsetjmp (ctrlc_buf, 1) != 0);

  while (!quit) {
    line = readline (prompt);
    if (!line)
      break;

    /* Removing trailing and leading whitespaces in place */
    g_strstrip (line);

    /* Skip empty lines */
    if (*line == '\0') {
      g_free (line);
      continue;
    }

    add_history (line);
    gstd_client_execute (line, data);

    g_free (line);
  }

  /* Free everything up */
  g_free (address);
  g_object_unref (data->client);
  if (data->con)
    g_object_unref (data->con);
  g_free (data);

  write_history (history_full);
  g_free (history_full);

  return EXIT_SUCCESS;
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
static gchar *
gstd_client_completer (const gchar * text, gint state)
{
  static gint list_index, len;
  const gchar *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state) {
    list_index = 0;
    len = strlen (text);
  }

  /* Return the next name which partially matches from the command list. */

  while ((name = cmds[list_index++].name))
    if (!strncmp (name, text, len))
      return g_strdup (name);

  /* If no names matched, then return NULL. */
  return NULL;
}

/* GNU FSF recommendations for open source */
static void
gstd_client_header (gboolean quiet)
{
  const gchar *header =
      "GStreamer Daemon  Copyright (C) 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)\n"
      "This program comes with ABSOLUTELY NO WARRANTY; for details type `warranty'.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; read the license for more details.\n";
  if (!quiet)
    g_print ("%s", header);
}

/* GNU FSF recommendations for open source */
static gint
gstd_client_cmd_warranty (gchar * name, gchar * arg, GstdClientData * data)
{
  const gchar *warranty = ""
      "Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)\n"
      "\n"
      "This library is free software; you can redistribute it and/or\n"
      "modify it under the terms of the GNU Library General Public\n"
      "License as published by the Free Software Foundation; either\n"
      "version 2 of the License, or (at your option) any later version.\n"
      "\n"
      "This library is distributed in the hope that it will be useful,\n"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
      "Library General Public License for more details.\n"
      "\n"
      "You should have received a copy of the GNU Library General Public\n"
      "License along with this library; if not, write to the\n"
      "Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,\n"
      "Boston, MA 02110-1301, USA.\n";

  g_print ("%s", warranty);
  return 0;
}

/* Print the help for all or a particular command */
static gint
gstd_client_cmd_help (gchar * name, gchar * arg, GstdClientData * data)
{
  GstdClientCmd *cmd = cmds;
  static gint maxlen = 0;
  gint curlen;
  const gchar *t1 = "COMMAND";
  const gchar *t2 = "DESCRIPTION";

  g_return_val_if_fail (arg, -1);

  /* One time max length calculation */
  if (!maxlen) {
    maxlen = strlen (t1);
    while (cmd->name) {
      curlen = strlen (cmd->usage);
      if (curlen > maxlen)
        maxlen = curlen;
      cmd++;
    }
    cmd = cmds;
  }

  /* Cryptic way to maintain the table column length for all rows */
#define FMT_TABLE(c1,c2) "%s%*s%s\n", c1, 4+maxlen-(gint)strlen(c1), " ", c2
  g_print (FMT_TABLE (t1, t2));
  while (cmd->name) {
    if ('\0' == arg[0] || !strcmp (arg, cmd->name))
      g_print (FMT_TABLE (cmd->usage, cmd->doc));
    cmd++;
  }

  return 0;
}

static void
gstd_client_process_error (GError * error)
{
  g_return_if_fail (error);

  g_printerr ("%s\n", error->message);
  g_error_free (error);
}

static gint
gstd_client_cmd_socket (gchar * name, gchar * arg, GstdClientData * data)
{
  gchar *cmd;
  GError *err = NULL;
  GInputStream *istream;
  GOutputStream *ostream;
  gchar buffer[1024];
  GString *response = NULL;
  gchar *array = NULL;
  gint read = 0;
  gint acc_read = 0;
  gchar *path_name;
  const gchar terminator = '\0';
  gint ret = -1;

  g_return_val_if_fail (name, -1);
  g_return_val_if_fail (arg, -1);
  g_return_val_if_fail (data, -1);

  if (data->use_unix) {
    GSocketAddress *socket_address;
    g_socket_client_set_family (data->client, G_SOCKET_FAMILY_UNIX);
    path_name = g_strdup_printf ("%s_%d", data->address, data->unix_port);
    socket_address = g_unix_socket_address_new (path_name);
    g_free (path_name);

    data->con = g_socket_client_connect (data->client,
        (GSocketConnectable *) socket_address, NULL, &err);
  } else {
    data->con = g_socket_client_connect_to_host (data->client,
        data->address, data->tcp_port, NULL, &err);
  }

  if (err) {
    gstd_client_process_error (err);
    goto out;
  }

  istream = g_io_stream_get_input_stream (G_IO_STREAM (data->con));
  ostream = g_io_stream_get_output_stream (G_IO_STREAM (data->con));

  cmd = g_strconcat (name, " ", arg, NULL);

  err = NULL;
  g_output_stream_write (ostream, cmd, strlen (cmd), NULL, &err);
  g_free (cmd);

  if (err) {
    gstd_client_process_error (err);
    goto write_error;
  }
  //Paranoia flush
  g_output_stream_flush (ostream, NULL, NULL);

  response = g_string_new ("");

  do {
    read = g_input_stream_read (istream, &buffer, sizeof (buffer), NULL, &err);
    if (err) {
      gstd_client_process_error (err);
      goto read_error;
    }

    g_string_append_len (response, buffer, read);

    acc_read += read;
    if (acc_read >= GSTD_CLIENT_MAX_RESPONSE) {
      g_set_error (&err, quark, -1,
          "Response exceeded %d bytes limit, probably the trailing "
          "NULL character was missing.", GSTD_CLIENT_MAX_RESPONSE);
      gstd_client_process_error (err);
      goto read_error;
    }

  } while (buffer[read - 1] != terminator);

  array = g_string_free (response, FALSE);
  g_print ("%s\n", array);
  g_free (array);

  ret = 0;
  goto out;

read_error:
  g_string_free (response, TRUE);

write_error:
  g_io_stream_close (G_IO_STREAM (data->con), NULL, NULL);
  g_object_unref (data->con);
  data->con = NULL;

out:
  return ret;
}

static gint
gstd_client_cmd_sh (gchar * name, gchar * arg, GstdClientData * data)
{
  GError *err = NULL;
  gchar **tokens;
  gint exit_status;

  g_return_val_if_fail (arg, -1);

  tokens = g_strsplit (arg, " ", -1);

  g_spawn_sync (NULL,           //CWD
      tokens,                   //argv
      NULL,                     //envp
      G_SPAWN_SEARCH_PATH_FROM_ENVP | G_SPAWN_CHILD_INHERITS_STDIN, NULL,       //child_setup
      NULL,                     //user_data
      NULL,                     //stdout
      NULL,                     //stderr
      &exit_status, &err);
  g_strfreev (tokens);

  if (err) {
    g_print ("%s\n", err->message);
    g_error_free (err);
  }

  return exit_status;
}

static gint
gstd_client_cmd_source (gchar * name, gchar * arg, GstdClientData * data)
{
  FILE *script;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  g_return_val_if_fail (arg, -1);
  g_return_val_if_fail (data, -1);

  script = fopen (arg, "r");
  if (!script) {
    g_printerr ("%s\n", strerror (errno));
    return -1;
  }

  while ((read = getline (&line, &len, script)) != -1) {
    if (!line)
      break;

    /* Removing trailing and leading whitespaces in place */
    g_strstrip (line);

    if (!data->quiet)
      g_print ("%s %s\n", data->prompt, line);

    /* Skip empty lines */
    if (*line == '\0')
      continue;

    add_history (line);
    gstd_client_execute (line, data);

    g_free (line);
    line = NULL;
  }

  fclose (script);

  return EXIT_SUCCESS;
}

static gint
gstd_client_cmd_quit (gchar * name, gchar * arg, GstdClientData * data)
{
  quit = TRUE;
  return 0;
}
