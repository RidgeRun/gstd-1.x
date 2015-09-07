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

typedef struct _GstdPipeline GstdPipeline;

struct _GstdPipeline
{
  guint index;
  gchar *name;
  GstPipeline *pipeline;
  gchar *description;
};

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
 * Iterates over the existing pipelines and returns the highest index
 * found in the array.
 *
 * \param pipelines A hash table of the existing pipelines.
 *
 * \return The highest index found or -1 if no pipelines where found.
 */
gint
gstd_pipeline_get_highest_index (GHashTable *pipelines);

#endif // __GSTD_PIPELINE_H__
