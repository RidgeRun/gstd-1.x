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

/* VTable */
static void
gstd_pipeline_iter_max (gpointer key, gpointer value, gpointer user_data);

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

  if (pipe->pipeline)
    g_object_unref(pipe->pipeline);

  g_free(pipe);
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

  
  GST_LOG ("Analyzing %s's index: %d vs current max %d", key, value->index, *outmax);
  if (value->index > *outmax)
    *outmax = value->index;
}

gint
gstd_pipeline_get_highest_index (GHashTable *pipelines)
{
  gint outmax;
  
  if (!pipelines) 
    goto no_pipelines;

  outmax = 0;
  g_hash_table_foreach (pipelines, gstd_pipeline_iter_max, &outmax);
  GST_DEBUG ("Found current max index to be %d", outmax);

  return outmax;

 no_pipelines:
  {
    GST_DEBUG ("No pipelines yet");
    return -1;
  }
}
