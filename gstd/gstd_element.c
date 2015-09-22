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
#include "gstd_element.h"

typedef struct _ForEachData ForEachData;

struct _ForEachData {
  GstdPipeline *pipeline;
  GList *elements;
};

/* VTable */
static void
_for_each (gpointer, gpointer);

static void
_for_each (gpointer data, gpointer user)
{
  GstElement *gstelement;
  GstdElement *gstdelement;
  ForEachData *fedata;

  gstelement = GST_ELEMENT(data);
  fedata = (ForEachData *)user;

  gstdelement = (GstdElement*)g_malloc(sizeof(GstdElement));
  gstdelement->element = gstelement;
  gstdelement->pipeline = fedata->pipeline;
  
  fedata->elements = g_list_append (fedata->elements, gstdelement);
}

GstdReturnCode
gstd_element_get_list (GstdPipeline *pipeline, GList **elements)
{
  ForEachData data;
  
  g_return_val_if_fail(pipeline, GSTD_MISSING_INITIALIZATION);

  /* If this is not null, we want to advice the user a potential memory leak */
  g_warn_if_fail(!*elements);

  GST_INFO("Returning elements in pipeline \"%s\"",
	   GSTD_PIPELINE_NAME(pipeline));

  data.pipeline = pipeline;
  data.elements = NULL;
  
  g_list_foreach (GST_BIN_CHILDREN(GSTD_PIPELINE_PIPELINE(pipeline)),
		  _for_each, &data);

  *elements = data.elements;

  return GSTD_EOK;
}
