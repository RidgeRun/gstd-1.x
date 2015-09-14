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

/* Global list of pipelines */
static GHashTable *gstd_pipeline_list = NULL;

/* VTable */
static void
gstd_pipeline_iter_max (gpointer, gpointer, gpointer);
static gint
gstd_pipeline_get_highest_index (GHashTable *);
static void
gstd_pipeline_free_hash (gpointer);

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

GstdPipeline *
gstd_pipeline_new ()
{
  GstdPipeline *pipe;

  pipe = g_malloc0(sizeof(GstdPipeline));
  pipe->index = -1;

  return pipe;
}

void
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
gstd_pipeline_create (gchar *name, gchar *description, gchar **outname)
{
  GstdPipeline *gstd_pipeline;
  gint newindex;
  GError *error;

  g_return_val_if_fail (gstd_pipeline_list, GSTD_MISSING_INITIALIZATION);
  g_return_val_if_fail (description, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outname, GSTD_NOT_NULL_ARGUMENT);
  
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
    g_return_val_if_reached (GSTD_BAD_DESCRIPTION);
  }
 existing_name:
  {
    GST_ERROR ("The name %s already exists", gstd_pipeline->name);
    gstd_pipeline_free (gstd_pipeline);
    g_return_val_if_reached (GSTD_EXISTING_NAME);
  }
}
