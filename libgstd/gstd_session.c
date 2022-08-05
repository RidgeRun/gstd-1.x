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

#include "gstd_session.h"
#include "gstd_list.h"
#include "gstd_tcp.h"
#include "gstd_pipeline_creator.h"
#include "gstd_property_reader.h"
#include "gstd_list_reader.h"
#include "gstd_pipeline_deleter.h"

/* Gstd Session debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_session_debug);
#define GST_CAT_DEFAULT gstd_session_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

GMutex singleton_mutex;

enum
{
  PROP_PIPELINES = 1,
  PROP_PID,
  PROP_DEBUG,
  N_PROPERTIES                  // NOT A PROPERTY
};

#define GSTD_SESSION_DEFAULT_PIPELINES NULL
#define GSTD_DEFAULT_PID -1

G_DEFINE_TYPE (GstdSession, gstd_session, GSTD_TYPE_OBJECT);

/* VTable */
static void
gstd_session_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_session_get_property (GObject *, guint, GValue *,
    GParamSpec *);
static void gstd_session_dispose (GObject *);
static GObject *gstd_session_constructor (GType, guint,
    GObjectConstructParam *);


static GObject *
gstd_session_constructor (GType type,
    guint n_construct_params, GObjectConstructParam * construct_params)
{
  static GObject *the_session = NULL;
  GObject *object = NULL;
  g_mutex_lock (&singleton_mutex);

  if (the_session == NULL) {
    object =
        G_OBJECT_CLASS (gstd_session_parent_class)->constructor (type,
        n_construct_params, construct_params);
    the_session = object;

    /* NULL out the_session when no references remain, to ensure a new
       session is created in the next constuctor */
    g_object_add_weak_pointer (the_session, (gpointer) & the_session);
  } else {
    object = g_object_ref (G_OBJECT (the_session));
  }
  g_mutex_unlock (&singleton_mutex);

  return object;
}

static void
gstd_session_class_init (GstdSessionClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_session_set_property;
  object_class->get_property = gstd_session_get_property;
  object_class->dispose = gstd_session_dispose;
  object_class->constructor = gstd_session_constructor;

  properties[PROP_PIPELINES] =
      g_param_spec_object ("pipelines",
      "Pipelines",
      "The pipelines created by the user",
      GSTD_TYPE_LIST,
      G_PARAM_READWRITE |
      G_PARAM_STATIC_STRINGS |
      GSTD_PARAM_CREATE | GSTD_PARAM_READ | GSTD_PARAM_DELETE);

  properties[PROP_PID] =
      g_param_spec_int ("pid",
      "PID",
      "The session process identifier",
      G_MININT,
      G_MAXINT, GSTD_DEFAULT_PID, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_DEBUG] =
      g_param_spec_object ("debug",
      "Debug",
      "The debug object containing debug information",
      GSTD_TYPE_DEBUG, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_session_debug, "gstdsession", debug_color,
      "Gstd Session category");
}

static void
gstd_session_init (GstdSession * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd session");

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));

  self->pipelines =
      GSTD_LIST (g_object_new (GSTD_TYPE_LIST, "name", "pipelines", "node-type",
          GSTD_TYPE_PIPELINE, "flags",
          GSTD_PARAM_CREATE | GSTD_PARAM_READ | GSTD_PARAM_UPDATE |
          GSTD_PARAM_DELETE, NULL));

  gstd_object_set_creator (GSTD_OBJECT (self->pipelines),
      g_object_new (GSTD_TYPE_PIPELINE_CREATOR, NULL));

  gstd_object_set_reader (GSTD_OBJECT (self->pipelines),
      g_object_new (GSTD_TYPE_LIST_READER, NULL));

  gstd_object_set_deleter (GSTD_OBJECT (self->pipelines),
      g_object_new (GSTD_TYPE_PIPELINE_DELETER, NULL));

  self->debug =
      GSTD_DEBUG (g_object_new (GSTD_TYPE_DEBUG, "name", "Debug", NULL));

  self->pid = (GPid) getpid ();
}

static void
gstd_session_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdSession *self = GSTD_SESSION (object);

  switch (property_id) {
    case PROP_PIPELINES:
      GST_DEBUG_OBJECT (self, "Returning pipeline list %p", self->pipelines);
      g_value_set_object (value, self->pipelines);
      break;
    case PROP_PID:
      GST_DEBUG_OBJECT (self, "Returning pid %d", self->pid);
      g_value_set_int (value, self->pid);
      break;
    case PROP_DEBUG:
      GST_DEBUG_OBJECT (self, "Returning debug object %p", self->debug);
      g_value_set_object (value, self->debug);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_session_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdSession *self = GSTD_SESSION (object);

  switch (property_id) {
    case PROP_PIPELINES:
      self->pipelines = g_value_dup_object (value);
      GST_INFO_OBJECT (self, "Changed pipeline list to %p", self->pipelines);
      break;
    case PROP_DEBUG:
      self->debug = g_value_dup_object (value);
      GST_DEBUG_OBJECT (self, "Changing debug object to %p", self->debug);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_session_dispose (GObject * object)
{
  GstdSession *self = GSTD_SESSION (object);

  GST_INFO_OBJECT (object, "Deinitializing gstd session");

  if (self->pipelines) {
    g_object_unref (self->pipelines);
    self->pipelines = NULL;
  }

  if (self->debug) {
    g_object_unref (self->debug);
    self->debug = NULL;
  }

  G_OBJECT_CLASS (gstd_session_parent_class)->dispose (object);
}

GstdSession *
gstd_session_new (const gchar * name)
{
  GstdSession *self;
  if (!name) {
    GPid tempPid = (GPid) getpid ();
    gchar *pid_name = g_strdup_printf ("Session %d", tempPid);
    self =
        GSTD_SESSION (g_object_new (GSTD_TYPE_SESSION, "name", pid_name, NULL));
    g_free (pid_name);
  } else {
    self = GSTD_SESSION (g_object_new (GSTD_TYPE_SESSION, "name", name, NULL));
  }
  return self;
}

GstdReturnCode
gstd_get_by_uri (GstdSession * gstd, const gchar * uri, GstdObject ** node)
{
  GstdObject *parent, *child;
  gchar **nodes;
  gchar **it;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_SESSION (gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (uri, GSTD_NULL_ARGUMENT);

  nodes = g_strsplit_set (uri, "/", -1);

  if (!nodes)
    goto badcommand;

  it = nodes;
  parent = g_object_ref (GSTD_OBJECT (gstd));

  while (*it) {
    // Empty slash, try no normalize
    if ('\0' == *it[0]) {
      ++it;
      continue;
    }

    ret = gstd_object_read (parent, *it, &child);
    g_object_unref (parent);

    if (ret)
      goto nonode;

    parent = child;
    ++it;
  }

  g_strfreev (nodes);
  *node = parent;
  return GSTD_EOK;

badcommand:
  {
    GST_ERROR_OBJECT (gstd, "Invalid command");
    return GSTD_BAD_COMMAND;
  }
nonode:
  {
    GST_ERROR_OBJECT (gstd, "Invalid node %s", *it);
    g_strfreev (nodes);
    return GSTD_BAD_COMMAND;
  }
}
