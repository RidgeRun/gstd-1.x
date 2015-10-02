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
#include "gstd.h"
#include "gstd_list.h"
#include <string.h>
#include <stdio.h>


/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_core_debug);
#define GST_CAT_DEFAULT gstd_core_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

enum {
  PROP_PIPELINES = 1,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_CORE_DEFAULT_PIPELINES NULL

struct _GstdCore
{
  GstdObject parent;
  
  /**
   * The list of GstdPipelines created by the user
   */
  GList *pipelines;
};

G_DEFINE_TYPE (GstdCore, gstd_core, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_core_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_core_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_core_dispose (GObject *);

static void
gstd_core_class_init (GstdCoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_core_set_property;
  object_class->get_property = gstd_core_get_property;
  object_class->dispose = gstd_core_dispose;

  properties[PROP_PIPELINES] =
    g_param_spec_object ("pipelines",
			 "Pipelines",
			 "The pipelines created by the user",
			 GSTD_TYPE_LIST,
			 G_PARAM_READWRITE |
			 G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_CREATE |
			 GSTD_PARAM_READ |
			 GSTD_PARAM_DELETE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);


  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_core_debug, "gstdcore", debug_color,
			   "Gstd Core category");
}

static void
gstd_core_init (GstdCore *self)
{
  GST_INFO_OBJECT(self, "Initializing gstd core");

  self->pipelines = GSTD_CORE_DEFAULT_PIPELINES;
}

static void
gstd_core_get_property (GObject        *object,
			guint           property_id,
			GValue         *value,
			GParamSpec     *pspec)
{
  GstdCore *self = GSTD_CORE(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_PIPELINES:
    GST_DEBUG_OBJECT(self, "Returning pipeline list %p", self->pipelines);
    g_value_set_pointer (value, self->pipelines);
    break;
    
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_core_set_property (GObject      *object,
		   guint         property_id,
		   const GValue *value,
		   GParamSpec   *pspec)
{
  GstdCore *self = GSTD_CORE(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
  
  switch (property_id) {
  case PROP_PIPELINES:
    self->pipelines = g_value_get_pointer (value);
    GST_INFO_OBJECT(self, "Changed pipeline list to %p", self->pipelines);
    break;
    
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_core_dispose (GObject *object)
{
  GstdCore *self = GSTD_CORE(object);
  
  GST_INFO_OBJECT(object, "Deinitializing gstd core");

  if (self->pipelines) {
    g_list_free_full (self->pipelines, g_object_unref);
    self->pipelines = NULL;
  }
  
  G_OBJECT_CLASS(gstd_core_parent_class)->dispose(object);
}

GstdReturnCode
gstd_uri (GstdCore *gstd, const gchar *cmd)
{
  gchar **action;
  const gchar *daction = " ";
  gchar **path;
  const gchar *dpath = "/";
  GObject *parent;
  const gchar *uri;

  g_return_val_if_fail(GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail(cmd, GSTD_NULL_ARGUMENT);

  action = g_strsplit (cmd, daction, -1);

  if (!action[0] || !action[1])
    goto badcommand;

  uri = action[1];

  path = g_strsplit (uri, dpath, -1);

  
  parent = g_object_ref(gstd);
  
  if (!g_ascii_strcasecmp(action[0], "CREATE")) {
  } else if (!g_ascii_strcasecmp(action[0], "READ")) {
  } else if (!g_ascii_strcasecmp(action[0], "UPDATE")) {
  } else if (!g_ascii_strcasecmp(action[0], "DELETE")) {
  } else {
  }

  g_object_unref(parent);
  return GSTD_EOK;
  
 badcommand:
  {
    GST_ERROR_OBJECT(gstd, "Invalid command");
    return GSTD_BAD_COMMAND;
  }
}


