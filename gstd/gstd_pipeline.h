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
#include "return_codes.h"
#include "gstd_debug.h"

typedef struct _GstdPipeline GstdPipeline;

struct _GstdPipeline
{
  guint index;
  gchar *name;
  GstElement *pipeline;
  gchar *description;
};

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
 * Allocates a new GstdPipeline.
 *
 * Allocates and returns a new pointer to GstdPipeline. The pointer
 * must then be freed using gstd_pipeline_free.
 *
 * \return A newly allocated GstdPipeline
 */
GstdPipeline *
gstd_pipeline_new ();

/**
 * Frees a previously allocated GstdPipeline.
 * 
 * \param The pipeline to free
 *
 * \pre The pipeline must have been allocated using gstd_pipeline_new
 * \post The pipeline will no longer be usable
 */
void
gstd_pipeline_free (GstdPipeline *pipeline);

/**
 * Returns the list of current pipelines. Note that this list 
 * may be NULL if gstd_pipeline_init hasnt been called
 *
 * \return A GHashTable of the list of pipelines.
 */
GHashTable*
gstd_pipeline_get_list ();

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
gstd_pipeline_create (gchar *name, gchar *description, gchar **outname);


#endif // __GSTD_PIPELINE_H__
