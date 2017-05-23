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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <gst/gst.h>

#include "gstd_pipeline.h"
#include "gstd_object.h"
#include "gstd_list.h"
#include "gstd_element.h"
#include "gstd_event_handler.h"
#include "gstd_pipeline_bus.h"
#include "gstd_list_reader.h"
#include "gstd_property_reader.h"
#include "gstd_state.h"

enum
{
  PROP_DESCRIPTION = 1,
  PROP_ELEMENTS,
  PROP_PIPELINE_BUS,
  PROP_STATE,
  PROP_EVENT,
  N_PROPERTIES                  // NOT A PROPERTY
};

#define GSTD_PIPELINE_DEFAULT_DESCRIPTION NULL
#define GSTD_PIPELINE_DEFAULT_STATE GSTD_PIPELINE_NULL

/* Gstd Pipeline debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_debug);
#define GST_CAT_DEFAULT gstd_pipeline_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdPipeline:
 * A wrapper for the conventional pipeline
 */
struct _GstdPipeline
{
  GstdObject parent;

  /**
   * The GstLaunch syntax used to create the pipeline
   */
  gchar *description;

  /**
   * The gstd event handler for this pipeline
   */
  GstdEventHandler *event_handler;


  /**
   * The Gstd Bus callback for this pipeline
   */

  GstdPipelineBus *pipeline_bus;

  /**
   * A Gstreamer element holding the pipeline
   */
  GstElement *pipeline;

  /**
   * The list of GstdElement held by the pipeline
   */
  GstdList *elements;

  /**
   * The state of the GstPipeline
   */
  GstdState *state;
};

struct _GstdPipelineClass
{
  GstdObjectClass parent_class;
};

G_DEFINE_TYPE (GstdPipeline, gstd_pipeline, GSTD_TYPE_OBJECT);

/* VTable */
static void
gstd_pipeline_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_pipeline_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_pipeline_dispose (GObject *);
static GstdReturnCode
gstd_pipeline_create (GstdPipeline *, const gchar *, gint, const gchar *);
static GstdReturnCode gstd_pipeline_fill_elements (GstdPipeline *,
    GstElement *);

static void
gstd_pipeline_class_init (GstdPipelineClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_pipeline_set_property;
  object_class->get_property = gstd_pipeline_get_property;
  object_class->dispose = gstd_pipeline_dispose;

  properties[PROP_DESCRIPTION] =
      g_param_spec_string ("description",
      "Description",
      "The gst-launch like pipeline description",
      GSTD_PIPELINE_DEFAULT_DESCRIPTION,
      G_PARAM_CONSTRUCT_ONLY |
      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_ELEMENTS] =
      g_param_spec_object ("elements",
      "Elements",
      "The elements in the pipeline",
      GSTD_TYPE_LIST,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_PIPELINE_BUS] =
      g_param_spec_object ("bus",
      "Bus",
      "The bus callback for this element",
      GSTD_TYPE_PIPELINE_BUS,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_STATE] =
      g_param_spec_object ("state", "State",
      "The state of the pipeline",
      GSTD_TYPE_STATE,			   
      G_PARAM_READWRITE |
      G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ | GSTD_PARAM_UPDATE);

  properties[PROP_EVENT] =
      g_param_spec_object ("event", "Event",
      "The event handler of the pipeline",
      GSTD_TYPE_EVENT_HANDLER, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_debug, "gstdpipeline", debug_color,
      "Gstd Pipeline category");
}

static void
gstd_pipeline_init (GstdPipeline * self)
{
  GST_INFO_OBJECT (self, "Initializing pipeline");
  self->description = g_strdup (GSTD_PIPELINE_DEFAULT_DESCRIPTION);
  self->pipeline = NULL;
  self->event_handler = NULL;
  self->pipeline_bus = NULL;
  self->state = NULL;

  self->elements = g_object_new (GSTD_TYPE_LIST, "name", "elements",
      "node-type", GSTD_TYPE_ELEMENT, "flags", GSTD_PARAM_READ, NULL);

  gstd_object_set_reader (GSTD_OBJECT(self->elements),
      g_object_new (GSTD_TYPE_LIST_READER, NULL));
  gstd_object_set_reader (GSTD_OBJECT(self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));
}

GstdReturnCode
gstd_pipeline_build (GstdPipeline * object)
{
  GstdPipeline *self = object;
  GstdReturnCode ret;

  ret =
      gstd_pipeline_create (self, GSTD_OBJECT_NAME (self), 0,
      self->description);
  if (GSTD_EOK != ret)
    goto out;

  self->event_handler = gstd_event_handler_new (G_OBJECT (self->pipeline));
  if (!self->event_handler) {
    ret = GSTD_BAD_VALUE;
    goto out1;
  }

  self->pipeline_bus =
      gstd_pipeline_bus_new (gst_pipeline_get_bus (GST_PIPELINE
          (self->pipeline)));

  if (!self->pipeline_bus) {
    ret = GSTD_BAD_VALUE;
    goto out2;
  }

  goto out;

out2:
  g_object_unref (self->event_handler);
  self->event_handler = NULL;

out1:
  g_object_unref (self->elements);
  self->elements = NULL;
  g_object_unref (self->pipeline);
  self->pipeline = NULL;

out:
  return ret;
}

static void
gstd_pipeline_dispose (GObject * object)
{
  GstdPipeline *self = GSTD_PIPELINE (object);

  GST_INFO_OBJECT (self, "Disposing %s pipeline", GSTD_OBJECT_NAME (self));

  /* Stop the pipe if playing */
  if (self->state) {
    gstd_object_update (GSTD_OBJECT(self->state), "NULL");
    g_object_unref (self->state);
    self->state = NULL;
  }
  
  if (self->description) {
    g_free (self->description);
    self->description = NULL;
  }

  if (self->pipeline_bus) {
    g_object_unref (self->pipeline_bus);
    self->pipeline_bus = NULL;
  }

  if (self->event_handler) {
    g_object_unref (self->event_handler);
    self->event_handler = NULL;
  }

  if (self->pipeline) {
    gst_object_unref (self->pipeline);
    self->pipeline = NULL;
  }

  if (self->elements) {
    g_object_unref (self->elements);
    self->elements = NULL;
  }
  
  G_OBJECT_CLASS (gstd_pipeline_parent_class)->dispose (object);
}

static void
gstd_pipeline_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdPipeline *self = GSTD_PIPELINE (object);

  switch (property_id) {
    case PROP_DESCRIPTION:
      GST_DEBUG_OBJECT (self, "Returning description of \"%s\"",
          self->description);
      g_value_set_string (value, self->description);
      break;
    case PROP_ELEMENTS:
      GST_DEBUG_OBJECT (self, "Returning element list %p", self->elements);
      g_value_set_object (value, self->elements);
      break;
    case PROP_PIPELINE_BUS:
      GST_DEBUG_OBJECT (self, "Returning pipeline bus %p", self->pipeline_bus);
      g_value_set_object (value, self->pipeline_bus);
      // g_value_set_object (value, self->elements);
      break;
    case PROP_STATE:
      GST_DEBUG_OBJECT (self, "Returning pipeline state %p", self->state);
      g_value_set_object (value, self->state);
      break;
    case PROP_EVENT:
      GST_DEBUG_OBJECT (self, "Returning event handler %p",
          self->event_handler);
      g_value_set_object (value, self->event_handler);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_pipeline_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdPipeline *self = GSTD_PIPELINE (object);

  switch (property_id) {
    case PROP_DESCRIPTION:
      if (self->description)
        g_free (self->description);
      self->description = g_value_dup_string (value);
      GST_INFO_OBJECT (self, "Changed description to \"%s\"",
          self->description);
      break;
    case PROP_STATE:
      if (self->state) {
	g_object_unref (self->state);
      }
      self->state = g_value_get_object (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

/**
 * Creates a new named pipeline based on the provided gst-launch
 * description. If no name is provided then a generic name will be 
 * assigned.
 *
 * \param name A unique name to assign to the pipeline. If empty or
 * NULL, a unique name will be generated.  
 * \param description A gst-launch like description of the pipeline.  
 * \param newpipe A double pointer to hold the newly created GstdPipeline. 
 * It may be passed NULL to ignore output values. This pointer will be 
 * NULL in case of failure. Do not free this pointer!
 *
 * \return A GstdReturnCode with the return status.
 *
 * \post A new pipeline will be allocated with the given name.
 */
static GstdReturnCode
gstd_pipeline_create (GstdPipeline * self, const gchar * name,
    const gint index, const gchar * description)
{
  GError *error;
  const gchar *fbname = "pipeline%d";
  gchar *pipename;
  GstParseFlags flags;

  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (index != -1, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (description, GSTD_NULL_ARGUMENT);

  error = NULL;
  flags = GST_PARSE_FLAG_FATAL_ERRORS | GST_PARSE_FLAG_NO_SINGLE_ELEMENT_BINS;
  self->pipeline = gst_parse_launch_full (description, NULL, flags, &error);
  if (!self->pipeline)
    goto wrong_pipeline;

  /* Single element descriptions (i.e.: playbin) aren't returned in a
     pipeline. This is a problem for us since we concepts like the bus
     which are directly related to a GstPipeline */
  if (!GST_IS_PIPELINE(self->pipeline)) {
    GstElement * element = self->pipeline;
    self->pipeline = gst_pipeline_new (GST_OBJECT_NAME(element));
    gst_bin_add (GST_BIN(self->pipeline), element);
  }

  if (self->state) {
    g_object_unref (self->state);
  }
  self->state = gstd_state_new (self->pipeline);
  
  /* If the user didn't provide a name or provided an empty name
   * assign the fallback using the idex */
  if (!name || name[0] == '\0') {
    pipename = g_strdup_printf (fbname, index);
  } else {
    pipename = g_strdup (name);
  }

  /* Set the updated name */
  gst_object_set_name (GST_OBJECT (self->pipeline), pipename);
  g_free (pipename);

  GST_INFO_OBJECT (self, "Created pipeline \"%s\": \"%s\"",
      GSTD_OBJECT_NAME (self), description);

  return gstd_pipeline_fill_elements (self, self->pipeline);

wrong_pipeline:
  {
    if (error) {
      GST_ERROR_OBJECT (self, "Unable to create pipeline: %s", error->message);
      g_error_free (error);
    }
    return GSTD_BAD_DESCRIPTION;
  }
}

static GstdReturnCode
gstd_pipeline_fill_elements (GstdPipeline * self, GstElement * element)
{
  GstPipeline *pipe;
  GstIterator *it;
  GValue item = G_VALUE_INIT;
  GstElement *gste;
  gboolean done;
  GstdElement *gstd_element;

  g_return_val_if_fail (GSTD_IS_PIPELINE (self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GST_IS_ELEMENT (element), GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT (self, "Gathering \"%s\" elements", GSTD_OBJECT_NAME (self));

  if (!GST_IS_PIPELINE (element))
    goto singleelement;

  pipe = GST_PIPELINE (element);

  it = gst_bin_iterate_elements (GST_BIN (pipe));
  if (!it)
    goto noiter;

  done = FALSE;

  while (!done) {
    switch (gst_iterator_next (it, &item)) {
      case GST_ITERATOR_OK:
        gste = g_value_get_object (&item);
        GST_LOG_OBJECT (self, "Saving element \"%s\"", GST_OBJECT_NAME (gste));

        gstd_element = g_object_new (GSTD_TYPE_ELEMENT, "name",
            GST_OBJECT_NAME (gste), "gstelement", gste, NULL);
        gstd_list_append_child (self->elements, GSTD_OBJECT(gstd_element));

        g_value_reset (&item);
        break;
      case GST_ITERATOR_RESYNC:
        gst_iterator_resync (it);
        break;
      case GST_ITERATOR_ERROR:
        GST_ERROR_OBJECT (self, "Unknown element iterator error");
        done = TRUE;
        break;
      case GST_ITERATOR_DONE:
        done = TRUE;
        break;
    }
  }
  g_value_unset (&item);
  gst_iterator_free (it);

  GST_DEBUG_OBJECT (self, "Elements where saved");

  return GSTD_EOK;

singleelement:
  {
    GST_INFO_OBJECT (self, "The pipeline \"%s\" doesn't contain elements!",
        GSTD_OBJECT_NAME (self));
    return GSTD_EOK;
  }
noiter:
  {
    GST_ERROR_OBJECT (self, "Malformed pipeline \"%s\"",
        GSTD_OBJECT_NAME (self));
    return GSTD_NO_PIPELINE;
  }
}
