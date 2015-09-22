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
#include <string.h>

/* Global list of pipelines */
static GHashTable *gstd_pipeline_list = NULL;

/* VTable */
static void
gstd_pipeline_iter_max (gpointer, gpointer, gpointer);
static gint
gstd_pipeline_get_highest_index (GHashTable *);
static void
gstd_pipeline_free_hash (gpointer);
static GstdPipeline *
gstd_pipeline_new (void);
static void
gstd_pipeline_free (GstdPipeline *pipeline);
static GstdReturnCode
gstd_pipeline_get (GHRFunc, const gpointer, GstdPipeline **);
static gboolean
_get_by_name (gpointer, gpointer, gpointer);
static gboolean
_get_by_index (gpointer, gpointer, gpointer);


void
gstd_pipeline_init ()
{
  /* Initialize the global pipeline list */
  if (!gstd_pipeline_list) {
    gstd_pipeline_list = g_hash_table_new_full (g_str_hash, g_str_equal,
        g_free, gstd_pipeline_free_hash);
    GST_DEBUG ("Initialized pipelines hash table");
  }
}

void
gstd_pipeline_deinit ()
{
  g_return_if_fail (gstd_pipeline_list);

  g_hash_table_unref (gstd_pipeline_list);
  gstd_pipeline_list = NULL;
  GST_DEBUG ("Deinitialized pipeline hash table");
}

/*
 * Allocates a new GstdPipeline.
 *
 * Allocates and returns a new pointer to GstdPipeline. The pointer
 * must then be freed using gstd_pipeline_free.
 *
 * \return A newly allocated GstdPipeline
 */

static GstdPipeline *
gstd_pipeline_new ()
{
  GstdPipeline *pipe;

  pipe = g_malloc0(sizeof(GstdPipeline));
  pipe->index = -1;

  return pipe;
}

/*
 * Frees a previously allocated GstdPipeline.
 * 
 * \param The pipeline to free
 *
 * \pre The pipeline must have been allocated using gstd_pipeline_new
 * \post The pipeline will no longer be usable
 */
static void
gstd_pipeline_free (GstdPipeline *pipe)
{
  g_return_if_fail (pipe);

  if (pipe->name) {
    GST_DEBUG ("Freeing pipeline %s", pipe->name);
    g_free(pipe->name);
  }

  if (pipe->description)
    g_free(pipe->description);

  if (pipe->pipeline)
    g_object_unref(pipe->pipeline);

  g_free(pipe);
}

void
gstd_pipeline_free_hash (gpointer pipe)
{
  /* Shush the compiler about different expected argument type */
  gstd_pipeline_free ((GstdPipeline *)pipe);
}

GHashTable *
gstd_pipeline_get_list ()
{
  g_return_val_if_fail (gstd_pipeline_list, NULL);
  
  return gstd_pipeline_list;
}

static void
gstd_pipeline_iter_max (gpointer _key, gpointer _value, gpointer _user_data)
{
  gchar *key = (gchar *)_key;
  GstdPipeline *value = (GstdPipeline *)_value;
  gint *outmax = (gint *)_user_data;

  g_return_if_fail(key);
  g_return_if_fail(value);
  g_return_if_fail(outmax);
  
  GST_LOG ("Analyzing %s's index: %d vs current max %d", key,
	   value->index, *outmax);

  if (value->index > *outmax)
    *outmax = value->index;
}

static gint
gstd_pipeline_get_highest_index (GHashTable *pipelines)
{
  gint outmax;

  outmax = -1;
  g_return_val_if_fail (pipelines, outmax);

  g_hash_table_foreach (pipelines, gstd_pipeline_iter_max, &outmax);
  GST_DEBUG ("Found current max index to be %d", outmax);

  return outmax;
}

GstdReturnCode
gstd_pipeline_create (const gchar *name, const gchar *description,
		      gchar **outname)
{
  GstdPipeline *gstd_pipeline;
  gint newindex;
  GError *error;

  g_return_val_if_fail (gstd_pipeline_list, GSTD_MISSING_INITIALIZATION);
  g_return_val_if_fail (description, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outname);
  
  gstd_pipeline = gstd_pipeline_new();

  newindex = gstd_pipeline_get_highest_index (gstd_pipeline_list);
  newindex++;
  gstd_pipeline->index = newindex;
  
  if (!name || name[0] == '\0') {
    gstd_pipeline->name = g_strdup_printf("pipeline%d",newindex);
  } else {
    gstd_pipeline->name = g_strdup(name);
  }

  if (g_hash_table_contains (gstd_pipeline_list, gstd_pipeline->name))
    goto existing_name;
  
  error = NULL;
  gstd_pipeline->pipeline = gst_parse_launch(description, &error);
  if (!gstd_pipeline->pipeline)
    goto wrong_pipeline;

  /* Still check if error is set because a recoverable
   * error might have occured */
  if (error) {
    GST_WARNING ("Recoverable error: %s", error->message);
    g_error_free (error);
  }
  gstd_pipeline->description = g_strdup(description);

  /* Pipelines do not have parents, so this function cant fail */
  gst_object_set_name (GST_OBJECT(gstd_pipeline->pipeline),
		       gstd_pipeline->name);
  
  *outname = gstd_pipeline->name;
  GST_INFO ("Created pipeline \"%s\": \"%s\"", *outname, description);
  g_hash_table_insert (gstd_pipeline_list, g_strdup(gstd_pipeline->name),
		       gstd_pipeline);
  
  return GSTD_EOK;

 wrong_pipeline:
  {
    if (error) {
      GST_ERROR ("Unable to create pipeline: %s", error->message);
      g_error_free (error);
    }
    else
      GST_ERROR ("Unable to create pipeline");

    gstd_pipeline_free (gstd_pipeline);
    return GSTD_BAD_DESCRIPTION;
  }
 existing_name:
  {
    GST_ERROR ("The name %s already exists", gstd_pipeline->name);
    gstd_pipeline_free (gstd_pipeline);
    return GSTD_EXISTING_NAME;
  }
}

GstdReturnCode
gstd_pipeline_destroy (const gchar *name)
{
  g_return_val_if_fail(gstd_pipeline_list, GSTD_MISSING_INITIALIZATION);
  g_return_val_if_fail(name, GSTD_NULL_ARGUMENT);
  
  if (!g_hash_table_remove (gstd_pipeline_list, name))
    goto not_found;

  GST_INFO ("Removed pipeline \"%s\"", name);
  return GSTD_EOK;
  
 not_found:
  GST_ERROR("Pipeline with name \"%s\": was not found in the pipeline list",
	    name);
  return GSTD_NO_PIPELINE;
}


GstdReturnCode
gstd_pipeline_get (GHRFunc getter, const gpointer data,
		   GstdPipeline **outpipe)
{
  /* Pointer Guards */
  g_return_val_if_fail(gstd_pipeline_list, GSTD_MISSING_INITIALIZATION);
  g_return_val_if_fail(data, GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outpipe);

  g_hash_table_ref(gstd_pipeline_list);
  *outpipe = (GstdPipeline *)g_hash_table_find(gstd_pipeline_list, getter,
					      data);
  g_hash_table_unref(gstd_pipeline_list);

  if (!*outpipe)
    goto nopipe;

  return GSTD_EOK;

 nopipe:
  {
    GST_WARNING("No pipeline found for the given criteria");
    return GSTD_NO_PIPELINE;
  }
}

static gboolean _get_by_name (gpointer key, gpointer value, gpointer data)
{
  GstdPipeline *pipe;
  gchar *name;

  pipe = (GstdPipeline *)value;
  name = (gchar *)data;

  GST_LOG("Comparing \"%s\" vs \"%s\"",  name, pipe->name);
  
  return !strcmp(name, pipe->name);
}
static gboolean _get_by_index (gpointer key, gpointer value, gpointer data)
{
  GstdPipeline *pipe;
  gint index;

  pipe = (GstdPipeline *)value;
  index = *(gint*)data;

  GST_LOG("Comparing %d vs %d",  index, pipe->index);

  return index == pipe->index;
}

GstdReturnCode
gstd_pipeline_get_by_name (const gchar *name, GstdPipeline **outpipe)
{
  GST_INFO("Looking for pipeline \"name\"", name);
  return gstd_pipeline_get(_get_by_name, (gpointer)name, outpipe);
}

GstdReturnCode
gstd_pipeline_get_by_index (const gint index, GstdPipeline **outpipe)
{
  GST_INFO("Looking for pipeline number %d", index);
  return gstd_pipeline_get(_get_by_index, (gpointer)&index, outpipe);
}
