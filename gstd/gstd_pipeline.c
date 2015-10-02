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
#include "gstd_pipeline.h"
#include "gstd_element.h"
#include <string.h>

enum {
  PROP_INDEX = 1,
  PROP_DESCRIPTION,
  PROP_ELEMENTS,
  N_PROPERTIES // NOT A PROPERTY
};

#define GSTD_PIPELINE_DEFAULT_INDEX -1
#define GSTD_PIPELINE_DEFAULT_DESCRIPTION NULL
#define GSTD_PIPELINE_DEFAULT_ELEMENTS NULL

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_pipeline_debug);
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
   * The unique, numerical id for the current pipeline
   */
  gint index;

  /**
   * The GstLaunch syntax used to create the pipeline
   */
  gchar *description;
  
  /**
   * A Gstreamer element holding the pipeline
   */
  GstElement *pipeline;

  /**
   * The list of GstdElement held by the pipeline
   */
  GList *elements;
};

G_DEFINE_TYPE (GstdPipeline, gstd_pipeline, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_pipeline_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_pipeline_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_pipeline_dispose (GObject *);
static GObject *
gstd_pipeline_constructor (GType , guint, GObjectConstructParam *);
static GstdReturnCode
gstd_pipeline_create (GstdPipeline *, const gchar *, const gint, const gchar *);
static GstdReturnCode
gstd_pipeline_fill_elements (GstdPipeline *, GstElement *);

static void
gstd_pipeline_class_init (GstdPipelineClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_pipeline_set_property;
  object_class->get_property = gstd_pipeline_get_property;
  object_class->constructor = gstd_pipeline_constructor;
  object_class->dispose = gstd_pipeline_dispose;
  
  properties[PROP_INDEX] =
    g_param_spec_int ("index",
		      "Index",
		      "The numerical index of the pipeline",
		      G_MININT,
		      G_MAXINT,
		      GSTD_PIPELINE_DEFAULT_INDEX,
		      G_PARAM_CONSTRUCT_ONLY |
		      G_PARAM_READWRITE |
		      G_PARAM_STATIC_STRINGS |
		      GSTD_PARAM_READ);

  properties[PROP_DESCRIPTION] =
    g_param_spec_string ("description",
                         "Description",
                         "The gst-launch like pipeline description",
			 GSTD_PIPELINE_DEFAULT_DESCRIPTION,
			 G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_READ);

  properties[PROP_ELEMENTS] =
    g_param_spec_pointer ("elements",
			  "Elements",
			  "The elements in the pipeline",
			  G_PARAM_READABLE |
			  G_PARAM_STATIC_STRINGS |
			  GSTD_PARAM_READ);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_debug, "gstdpipeline", debug_color,
			   "Gstd Pipeline category");
}

static void
gstd_pipeline_init (GstdPipeline *self)
{
  GST_INFO_OBJECT(self, "Initializing pipeline");
  self->index = GSTD_PIPELINE_DEFAULT_INDEX;
  self->description = g_strdup(GSTD_PIPELINE_DEFAULT_DESCRIPTION);
  self->pipeline = NULL;
  self->elements = GSTD_PIPELINE_DEFAULT_ELEMENTS;
}

static void
gstd_pipeline_dispose (GObject *object)
{
  GstdPipeline *self = GSTD_PIPELINE(object);

  GST_INFO_OBJECT(self, "Disposing %s pipeline", GSTD_OBJECT_NAME(self));
  
  if (self->description) {
    g_free(self->description);
    self->description = NULL;
  }

  if (self->pipeline) {
    g_object_unref(self->pipeline);
    self->pipeline = NULL;
  }

  if (self->elements) {
    g_list_free_full(self->elements, g_object_unref);
    self->elements = NULL;
  }

  G_OBJECT_CLASS(gstd_pipeline_parent_class)->dispose(object);
}

static GObject *
gstd_pipeline_constructor (GType type, guint count,
			   GObjectConstructParam *properties)
{
  GstdPipeline *self;
  gint index;
  const gchar *description;
  GstdReturnCode code;

  self = GSTD_PIPELINE(G_OBJECT_CLASS(gstd_pipeline_parent_class)->
		       constructor(type, count, properties));

  /* After chaining parent constructor the CONSTRUCT_ONLY properties
     have been already set */
  index = self->index;
  description = self->description;
  
  GST_DEBUG_OBJECT(self, "Called pipeline constructor with index: "
		   "\"%d\" and description : \"%s\"", self->index,
		   self->description);

  code = gstd_pipeline_create (self, GSTD_OBJECT_NAME(self),
			       self->index, self->description);
  gstd_object_set_code (GSTD_OBJECT(self), code);
  
  return G_OBJECT(self);
}

static void
gstd_pipeline_get_property (GObject        *object,
			    guint           property_id,
			    GValue         *value,
			    GParamSpec     *pspec)
{
  GstdPipeline *self = GSTD_PIPELINE(object);

  switch (property_id) {
  case PROP_INDEX:
    GST_DEBUG_OBJECT(self, "Returning index of %d", self->index);
    g_value_set_int (value, self->index);
    break;
  case PROP_DESCRIPTION:
    GST_DEBUG_OBJECT(self, "Returning description of \"%s\"",
		     self->description);
    g_value_set_string (value, self->description);
    break;
  case PROP_ELEMENTS:
    GST_DEBUG_OBJECT(self, "Returning element list %p", self->elements);
    g_value_set_pointer (value, self->elements);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    return;
  }

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
}

static void
gstd_pipeline_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  GstdPipeline *self = GSTD_PIPELINE (object);
  
  switch (property_id) {
  case PROP_INDEX:
    self->index = g_value_get_int(value);
    GST_INFO_OBJECT(self, "Changed index to \"%d\"", self->index);
    break;
  case PROP_DESCRIPTION:
    if (self->description)
      g_free (self->description);
    self->description = g_value_dup_string (value);
    GST_INFO_OBJECT(self, "Changed description to \"%s\"", self->description);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    return;
  }

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
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
gstd_pipeline_create (GstdPipeline *self, const gchar *name,
		      const gint index, const gchar *description)
{
  GError *error;
  const gchar *fbname = "pipeline%d";
  gchar *pipename;

  g_return_val_if_fail (self, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (index != -1, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (description, GSTD_NULL_ARGUMENT);
  
  error = NULL;
  self->pipeline = gst_parse_launch(description, &error);
  if (!self->pipeline)
    goto wrong_pipeline;

  /* Still check if error is set because a recoverable
   * error might have occured */
  if (error) {
    GST_WARNING_OBJECT (self, "Recoverable error: %s", error->message);
    g_error_free (error);
  }

  /* If the user didn't provide a name or provided an empty name
   * assign the fallback using the idex */
  if (!name || name[0] == '\0') {
    pipename = g_strdup_printf(fbname, index);
  } else {
    pipename = g_strdup (name);
  }

  /* Set the updated name */
  gst_object_set_name (GST_OBJECT(self->pipeline), pipename);
  g_object_set (self, "name", pipename, NULL);
  g_free (pipename);

  GST_INFO_OBJECT (self, "Created pipeline \"%s\": \"%s\"",
		   GSTD_OBJECT_NAME(self), description);

  return gstd_pipeline_fill_elements (self, self->pipeline);

 wrong_pipeline:
  {
    if (error) {
      GST_ERROR_OBJECT (self, "Unable to create pipeline: %s", error->message);
      g_error_free (error);
    }
    else
      GST_ERROR_OBJECT (self, "Unable to create pipeline");

    return GSTD_BAD_DESCRIPTION;
  }
 }

static GstdReturnCode
gstd_pipeline_fill_elements (GstdPipeline *self, GstElement *element)
{
  GstPipeline *pipe;
  GList *elementlist;
  GstIterator *it;
  GValue item = G_VALUE_INIT;
  GstElement *gste;
  GstdElement *gstde;
  gboolean done;

  g_return_val_if_fail (GSTD_IS_PIPELINE(self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GST_IS_ELEMENT(element), GSTD_NULL_ARGUMENT);

  GST_DEBUG_OBJECT(self, "Gathering \"%s\" elements", GSTD_OBJECT_NAME(self));
  
  if (!GST_IS_PIPELINE(element))
    goto singleelement;
      
  pipe = GST_PIPELINE(element);
  elementlist = self->elements;

  it = gst_bin_iterate_elements (GST_BIN(pipe));
  if (!it)
    goto noiter;

  done = FALSE;

  while (!done) {
    switch (gst_iterator_next (it, &item)) {
    case GST_ITERATOR_OK:
      gste = g_value_get_object (&item);
      GST_LOG_OBJECT(self,
          "Saving element \"%s\"", GST_OBJECT_NAME(gste));
      gstde = GSTD_ELEMENT(g_object_new(GSTD_TYPE_ELEMENT,
					"name", GST_OBJECT_NAME(gste),
					"properties", gste, NULL));
      elementlist = g_list_append (elementlist, gstde);
      g_value_reset (&item);
      break;
    case GST_ITERATOR_RESYNC:
	gst_iterator_resync (it);
      break;
    case GST_ITERATOR_ERROR:
      GST_ERROR_OBJECT(self, "Unknown element iterator error");
      done = TRUE;
      break;
    case GST_ITERATOR_DONE:
      done = TRUE;
      break;
    }
  }
  g_value_unset (&item);
  gst_iterator_free (it);

  GST_DEBUG_OBJECT(self, "A total of %u elements where saved",
		   g_list_length(elementlist));

  if (self->elements)
    g_list_free_full(self->elements, g_object_unref);

  self->elements = elementlist;
  
  return GSTD_EOK;
  
 singleelement:
  {
    GST_INFO_OBJECT(self, "The pipeline \"%s\" doesn't contain elements!",
		    GSTD_OBJECT_NAME(self));
    return GSTD_EOK;
  }
 noiter:
  {
    GST_ERROR_OBJECT(self, "Malformed pipeline \"%s\"", GSTD_OBJECT_NAME(self));
    return GSTD_NO_PIPELINE;
  }
}
