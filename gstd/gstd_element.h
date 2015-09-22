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

#ifndef __GSTD_ELEMENT_H__
#define __GSTD_ELEMENT_H__

#include <gst/gst.h>
#include "gstd_return_codes.h"
#include "gstd_debug.h"
#include "gstd_pipeline.h"

typedef struct _GstdElement GstdElement;

/**
 * GstdElement:
 * A wrapper over the conventional GstElement
 */
struct _GstdElement {
  /**
   * The associated Gstreamer element
   */
  GstElement *element;

  /**
   * The container GstdPipeline
   */
  GstdPipeline *pipeline;
};

/**
 * Returns the associated GStreamer element
 *
 * \param e A GstdElement
 * \return The associated Gstreamer element
 */
#define GSTD_ELEMENT_ELEMENT(e) ((e)->element)

/**
 * Returns the associated pipeline
 *
 * \param e A GstdElement
 * \return The associated GstdPipeline
 */
#define GSTD_ELEMENT_PIPELINE(e) ((e)->pipeline)

/**
 * Returns a list of the elements contained in the given GstdPipeline
 *
 * \param pipeline A valid GstdPipeline created by using gstd_pipeline_create
 * \param elements A double pointer to a GList to hold the element list. Must
 * be NULL. Free this list after usage by calling gstd_element_destroy_list()
 *
 * \return The appropriate return code.
 */
GstdReturnCode
gstd_element_get_list (GstdPipeline *pipeline, GList **elements);

/**
 * Properly frees a GstdElement list
 */
#define gstd_element_destroy_list (elements) \
  g_list_free_full(elements, g_free)

/**
 * Returns the element with the given name in the given pipeline
 *
 */
GstdReturnCode
gstd_element_get_by_name (const GstdPipeline *pipeline, const gchar *name,
			  GstdElement **element);


#endif // __GSTD_ELEMENT_H__
