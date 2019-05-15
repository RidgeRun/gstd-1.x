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
#include <gio/gunixsocketaddress.h>

#include "gstd_ipc.h"
#include "gstd_socket.h"
#include "gstd_unix.h"
#include "gstd_parser.h"
#include "gstd_element.h"
#include "gstd_pipeline_bus.h"
#include "gstd_event_handler.h"


/* Gstd TCP debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_unix_debug);
#define GST_CAT_DEFAULT gstd_unix_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO


struct _GstdUnix
{
  GstdSocket parent;
  gchar *unix_path;
  guint num_ports;
  GSocketService *service;
};

struct _GstdUnixClass
{
  GstdSocketClass parent_class;
};

G_DEFINE_TYPE (GstdUnix, gstd_unix, GSTD_TYPE_SOCKET);

enum
{
  PROP_UNIX_PATH = 1,
  PROP_UNIX_NUM_PORTS = 2,
  N_PROPERTIES                  // NOT A PROPERTY
};


/* VTable */

static void gstd_unix_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_unix_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_unix_dispose (GObject *);
GstdReturnCode
gstd_unix_add_listener_address (GstdSocket * base, GSocketService ** service);
gboolean gstd_unix_init_get_option_group (GstdIpc * base, GOptionGroup ** group);


static void
gstd_unix_class_init (GstdUnixClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdIpcClass *gstdipc_class = GSTD_IPC_CLASS (klass);
  GstdSocketClass* socket_class = GSTD_SOCKET_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;
  gchar *default_path;
  object_class->set_property = gstd_unix_set_property;
  object_class->get_property = gstd_unix_get_property;
  gstdipc_class->get_option_group =
      GST_DEBUG_FUNCPTR (gstd_unix_init_get_option_group);
  socket_class->add_listener_address =
      GST_DEBUG_FUNCPTR (gstd_unix_add_listener_address);
  object_class->dispose = gstd_unix_dispose;

  default_path = g_strdup_printf ("%s/%s", GSTD_RUN_STATE_DIR, GSTD_UNIX_DEFAULT_BASE_NAME);
  properties[PROP_UNIX_PATH] =
    g_param_spec_string ("unix-path",
                   "Unix path",
                   "The path used to create the unix socket address",
                   default_path,
                   G_PARAM_READWRITE |
                   G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS |
                   GSTD_PARAM_READ);
  g_free (default_path);

  properties[PROP_UNIX_NUM_PORTS] =
      g_param_spec_uint ("num-ports",
      "Num Ports",
      "The number of ports to open for the unix session, starting at unix-path",
      0,
      G_MAXINT,
      GSTD_UNIX_DEFAULT_NUM_PORTS,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_unix_debug, "gstdunix", debug_color,
      "Gstd UNIX category");
}

static void
gstd_unix_set_path (GstdUnix * self, const gchar * path)
{
  g_free (self->unix_path);

  if (path != NULL)
    self->unix_path = g_strdup (path);
  else
    self->unix_path = NULL;
}

static void
gstd_unix_init (GstdUnix * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd Unix");

  gchar *default_path;
  default_path = g_strdup_printf ("%s/%s", GSTD_RUN_STATE_DIR, GSTD_UNIX_DEFAULT_BASE_NAME);
  gstd_unix_set_path(self, default_path);
  g_free (default_path);
  self->num_ports = GSTD_UNIX_DEFAULT_NUM_PORTS;

}

static void
gstd_unix_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdUnix *self = GSTD_UNIX (object);

  switch (property_id) {
    case PROP_UNIX_PATH:
      GST_DEBUG_OBJECT (self, "Returning unix-path %s", self->unix_path);
      g_value_set_string (value, self->unix_path);
      break;

    case PROP_UNIX_NUM_PORTS:
      GST_DEBUG_OBJECT (self, "Returning number-ports %u", self->num_ports);
      g_value_set_uint (value, self->num_ports);
      break;

	  default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_unix_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdUnix *self = GSTD_UNIX (object);

  switch (property_id) {
    case PROP_UNIX_PATH:
      GST_DEBUG_OBJECT (self, "Changing unix-path current value: %s",
          self->unix_path);
      gstd_unix_set_path(self, g_value_get_string (value));
      break;

    case PROP_UNIX_NUM_PORTS:
      GST_DEBUG_OBJECT (self, "Changing num-ports current value: %u",
          self->num_ports);
      self->num_ports = g_value_get_uint (value);
      GST_DEBUG_OBJECT (self, "Value changed %u", self->num_ports);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}


static void
gstd_unix_dispose (GObject * object)
{
  GstdUnix *self = GSTD_UNIX (object);
  GstdIpc *parent = GSTD_IPC (object);
  guint i;

  GST_INFO_OBJECT (object, "Deinitializing gstd UNIX");

  if (parent->enabled) {
    for (i = 0; i < self->num_ports; i++) {
      gchar * path_name = g_strdup_printf ("%s_%d", self->unix_path, i);

      if (unlink(path_name) != 0) {
        GST_ERROR_OBJECT (object, "Unable to delete UNIX path (%s)", strerror(errno));
      }
      g_free (path_name);
    }
  }

  G_OBJECT_CLASS (gstd_unix_parent_class)->dispose (object);
}

GstdReturnCode
gstd_unix_add_listener_address (GstdSocket * base, GSocketService ** service)
{
  guint debug_color;
  GError *error = NULL;
  GstdUnix *self = GSTD_UNIX (base);
  gchar* path = self->unix_path;
  guint i;

  GST_DEBUG_OBJECT (self, "Getting UNIX Socket address");

  if (!gstd_unix_debug) {
    /* Initialize debug category with nice colors */
    debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
    GST_DEBUG_CATEGORY_INIT (gstd_unix_debug, "gstdunix", debug_color,
        "Gstd UNIX category");
  }

  *service = g_threaded_socket_service_new (self->num_ports);

  for (i = 0; i < self->num_ports; i++) {
    GSocketAddress *address;
    gchar * path_name = g_strdup_printf ("%s_%d", path, i);

    address = g_unix_socket_address_new (path_name);
	  g_free (path_name);

    g_socket_listener_add_address (G_SOCKET_LISTENER (*service),
                 address,
                 G_SOCKET_TYPE_STREAM,
                 G_SOCKET_PROTOCOL_DEFAULT,
                 NULL,
                 NULL,
                 &error);

      if (error)
        goto noconnection;
  }
  return GSTD_EOK;

noconnection:
  {
    GST_ERROR_OBJECT (*service, "%s", error->message);
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return GSTD_NO_CONNECTION;
  }
}

gboolean
gstd_unix_init_get_option_group (GstdIpc * base, GOptionGroup ** group)
{
  GstdUnix *self = GSTD_UNIX (base);
  GST_DEBUG_OBJECT (self, "UNIX init group callback ");
  GOptionEntry unix_args[] = {
    {"enable-unix-protocol", 'u', 0, G_OPTION_ARG_NONE, &base->enabled,
        "Enable attach the server through given UNIX socket ", NULL}
    ,
    {"unix-base-path", 'b', 0, G_OPTION_ARG_STRING, &self->unix_path,
          "Attach to the server using the given path (default /usr/local/var/run/gstd/gstd_default_unix_socket), " \
        "a '_<port_number>' is appended to this path to create the ports, for instance if only one port is created, its path will be /usr/local/var/run/gstd/gstd_default_unix_socket_0",
        "unix-path"}
    ,
    {"unix-num-ports", 'c', 0, G_OPTION_ARG_INT, &self->num_ports,
          "Number of ports to use starting at base-port (default 1)",
        "unix-num-ports"}
    ,
    {NULL}
  };
  *group = g_option_group_new ("gstd-unix", ("UNIX Options"),
      ("Show UNIX Options"), NULL, NULL);

  g_option_group_add_entries (*group, unix_args);
  return TRUE;
}
