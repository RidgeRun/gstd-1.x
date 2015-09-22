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

#ifndef __GSTD_PIPELINE_H__
#define __GSTD_PIPELINE_H__

#include <gst/gst.h>
#include "gstd_return_codes.h"
#include "gstd_debug.h"


typedef struct _GstdPipeline GstdPipeline;

/**
 * GstdPipeline:
 * A wrapper for the conventional pipeline
 */
struct _GstdPipeline
{
  /**
   * The unique, numerical id for the current pipeline
   */
  gint index;

  /**
   * A unique name for the pipeline
   */
  gchar *name;

  /**
   * A Gstreamer element holding the pipeline
   */
  GstElement *pipeline;

  /**
   * The GstLaunch syntax used to create the pipeline
   */
  gchar *description;
};

/**
 * Returns numerical index of the pipeline
 *
 * \param pipe a GstdPipeline
 * \return The numerical index of the pipeline
 */
#define GSTD_PIPELINE_INDEX(pipe) ((pipe)->index)

/**
 * Returns the name of the pipeline
 *
 * \param pipe a GstdPipeline
 * \return The name of the pipeline
 */
#define GSTD_PIPELINE_NAME(pipe) ((pipe)->name)

/**
 * Returns the Gstreamer pipeline
 *
 * \param pipe a GstdPipeline
 * \return The Gstreamer pipeline
 */
#define GSTD_PIPELINE_PIPELINE(pipe) ((pipe)->pipeline)

/**
 * Returns the Gstreamer pipeline
 *
 * \param pipe a GstdPipeline
 * \return The GstLaunch description used to create the pipeline
 */
#define GSTD_PIPELINE_DESCRIPTION(pipe) ((pipe)->description)

/**
 * Initializes the pipeline list. This has to be called prior
 * any operation on the pipeline list.
 */
void
gstd_pipeline_init ();

/**
 * Deinitializes the pipeline list. No pipeline operation may be
 * performed after this.
 */
void
gstd_pipeline_deinit ();

/**
 * Creates a new named pipeline based on the provided gst-launch
 * description. If no name is provided then a generic name will be 
 * assigned.
 *
 * \param name A unique name to assign to the pipeline. If empty or
 * NULL, a unique name will be generated.  
 * \param description A gst-launch like description of the pipeline.  
 * \param outname A pointer to char array to hold the name assigned 
 * to the pipeline. Must be NULL. This pointer will be NULL in case
 * of failure. Do not free this name!
 *
 * \return A GstdReturnCode with the return status.
 *
 * \post A new pipeline will be allocated with the given name.
 */
GstdReturnCode
gstd_pipeline_create (const gchar *name, const gchar *description,
		      gchar **outname);

/**
 * Destroys an existing pipeline.
 *
 * \param name The unique name assigned to the pipeline.
 *
 * \return A GstdReturnCode with the return status.
 *
 * \post The given pipeline resources will be freed.
 */
GstdReturnCode
gstd_pipeline_destroy (const gchar *name);

/**
 * Returns the hash table that currently holds the pipelines
 *
 * \return A hash table containing the pipelines with its names
 * as keys and GstdPipeline as values
 */
GHashTable *
gstd_pipeline_get_list ();

/**
 * Returns a read-only GstdPipeline by its name. This pipeline 
 * must not be modified. If the pipeline is not found, the 
 * return value will be NULL
 *
 * \param name The name of the pipeline to query
 * \param outpipe A read-only pointer to a GstdPipeline or NULL
 * if it wasn't found.
 */
GstdReturnCode
gstd_pipeline_get_by_name (const gchar *name, GstdPipeline **outpipe);

/**
 * Returns a read-only GstdPipeline by its index. This pipeline 
 * must not be modified. If the pipeline is not found, the 
 * return value will be NULL
 *
 * \param index The index of the pipeline to query
 * \param outpipe A read-only pointer to a GstdPipeline or NULL
 * if it wasn't found.
 */
GstdReturnCode
gstd_pipeline_get_by_index (const gint index, GstdPipeline **outpipe);

#endif // __GSTD_PIPELINE_H__
