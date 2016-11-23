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
#include "gstd_tcp.h"
#include <string.h>
#include <stdio.h>

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_core_debug);
#define GST_CAT_DEFAULT gstd_core_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

enum {
  PROP_PIPELINES = 1,
  PROP_PORT,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_CORE_DEFAULT_PIPELINES NULL

struct _GstdCore
{
  GstdObject parent;
  
  /**
   * The list of GstdPipelines created by the user
   */
  GstdList *pipelines;

  guint16 port;
  GSocketService *service;
};

struct _GstdCoreClass
{
  GstdObjectClass parent_class;
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
gstd_core_constructed (GObject *);

static void
gstd_core_class_init (GstdCoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_core_set_property;
  object_class->get_property = gstd_core_get_property;
  object_class->dispose = gstd_core_dispose;
  object_class->constructed = gstd_core_constructed;

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

  properties[PROP_PORT] =
    g_param_spec_uint ("port",
		       "Port",
		       "The port to start listening to",
		       0,
		       G_MAXINT,
		       GSTD_TCP_DEFAULT_PORT,
		       G_PARAM_READWRITE |
		       G_PARAM_CONSTRUCT_ONLY |
		       G_PARAM_STATIC_STRINGS |
		       GSTD_PARAM_READ);

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

  self->pipelines = GSTD_LIST(g_object_new(GSTD_TYPE_LIST, "name", "pipelines",
					   "node-type", GSTD_TYPE_PIPELINE, "flags",
					   GSTD_PARAM_CREATE | GSTD_PARAM_READ |
					   GSTD_PARAM_UPDATE | GSTD_PARAM_DELETE, NULL));

  //TODO:
  //gstd_list_set_creator(self->pipelines, g_object_new (GSTD_TYPE_PIPELINE_CREATOR);

  self->port = GSTD_TCP_DEFAULT_PORT;
  self->service = NULL;
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
    g_value_set_object (value, self->pipelines);
    break;
  case PROP_PORT:
    GST_DEBUG_OBJECT(self, "Returning post %u", self->port);
    g_value_set_uint (value, self->port);
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
    self->pipelines = g_value_get_object (value);
    GST_INFO_OBJECT(self, "Changed pipeline list to %p", self->pipelines);
    break;
  case PROP_PORT:
    GST_DEBUG_OBJECT(self, "Changing port to %u", self->port);
    self->port = g_value_get_uint (value);
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
    g_object_unref (self->pipelines);
    self->pipelines = NULL;
  }

  if (self->service) {
    gstd_tcp_stop (self, &self->service);
    self->service = NULL;
  }
  
  G_OBJECT_CLASS(gstd_core_parent_class)->dispose(object);
}

static void
gstd_core_constructed (GObject *object)
{
  GstdCore *self = GSTD_CORE(object);
  
  gstd_tcp_start (self, &self->service, self->port);
}


GstdCore *
gstd_new (const gchar *name, const guint16 port)
{
  return GSTD_CORE(g_object_new (GSTD_TYPE_CORE, "name", name, "port", port, NULL));
}

GstdReturnCode
gstd_pipeline_create (GstdCore *gstd, const gchar *name, const gchar *description)
{
  GstdObject *list;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (description, GSTD_NULL_ARGUMENT);

  gstd_object_read (GSTD_OBJECT(gstd), "pipelines", &list, NULL);
  ret =  gstd_object_create (list, name, description);
  g_object_unref (list);
  
  return ret;
}

GstdReturnCode
gstd_pipeline_destroy (GstdCore *gstd, const gchar *name)
{
  GstdObject *list;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  gstd_object_read (GSTD_OBJECT(gstd), "pipelines", &list, NULL);
  ret = gstd_object_delete (list, name);
  g_object_unref(list);

  return ret;
}

GstdReturnCode
gstd_pipeline_state (GstdCore *gstd, const gchar *pipe, const GstdPipelineState state)
{
  GstdObject *pipeline;
  gchar *uri;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipe, GSTD_NULL_ARGUMENT);

  uri = g_strdup_printf ("/pipelines/%s/", pipe);
  ret = gstd_get_by_uri (gstd, uri, &pipeline);
  if (ret)
    goto noelement;
  
  ret = gstd_object_update (pipeline, "state", state, NULL);
  
  g_object_unref(pipeline);
  g_free (uri);

  return ret;
  
 noelement:
  {
    return ret;
  }
}

GstdReturnCode
gstd_pipeline_play (GstdCore *gstd, const gchar *pipe)
{
  return gstd_pipeline_state (gstd, pipe, GSTD_PIPELINE_PLAYING);
}

GstdReturnCode
gstd_pipeline_null (GstdCore *gstd, const gchar *pipe)
{
  return gstd_pipeline_state (gstd, pipe, GSTD_PIPELINE_NULL);
}

typedef GstdReturnCode eaccess (GstdObject *, const gchar *, ...);
GstdReturnCode
gstd_element_generic (GstdCore *gstd, const gchar *pipe, const gchar *name,
		      const gchar *property, gpointer value, eaccess func)
{
  GstdObject *element;
  gchar *uri;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (pipe, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  element = NULL;
  uri =  g_strdup_printf ("/pipelines/%s/elements/%s/", pipe, name);
  ret = gstd_get_by_uri (gstd, uri, &element);
  g_free (uri);
  if (ret)
    goto baduri;

  ret = func (element, property, value, NULL);
  g_object_unref(element);

  return ret;

 baduri:
  {
    if (element)
      g_object_unref (element);
    return ret;
  }
}

GstdReturnCode
gstd_element_set (GstdCore *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value)
{
  return gstd_element_generic (gstd, pipe, name, property,
			       value, gstd_object_update);
}

GstdReturnCode
gstd_element_get (GstdCore *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value)
{
  return gstd_element_generic (gstd, pipe, name, property,
			       value, gstd_object_read);
}

GstdReturnCode
gstd_get_by_uri (GstdCore *gstd, const gchar *uri, GstdObject **node)
{
  GstdObject *parent, *child;
  gchar **nodes;
  gchar **it;
  GstdReturnCode ret;
  
  g_return_val_if_fail(GSTD_IS_CORE(gstd), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail(uri, GSTD_NULL_ARGUMENT);

  nodes = g_strsplit_set (uri, "/", -1);

  if (!nodes)
    goto badcommand;

  it = nodes;
  parent = g_object_ref(GSTD_OBJECT(gstd));
  
  while (*it) {
    // Empty slash, try no normalize
    if ('\0' == *it[0]) {
      ++it;
      continue;
    }
    
    ret = gstd_object_read (parent, *it, &child, NULL);
    g_object_unref (parent);

    if (ret)
      goto nonode;

    parent = child;
    ++it;
  }

  *node = parent;
  return GSTD_EOK;
  
 badcommand:
  {
    GST_ERROR_OBJECT(gstd, "Invalid command");
    return GSTD_BAD_COMMAND;
  }
 nonode:
  {
    GST_ERROR_OBJECT(gstd, "Invalid node %s", *it);
    return GSTD_BAD_COMMAND;
  }
}


