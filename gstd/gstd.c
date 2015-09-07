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
#include "gstd.h"

/* GSTD debugging category */
GST_DEBUG_CATEGORY (gstd_debug);

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/* List of pipeline references */
static GHashTable *gstd_pipelines = NULL;

void
gstd_init (gint *argc, gchar **argv[])
{
  guint debug_color;
  GstDebugLevel debug_level;
  
  /* Initialize debug category and setting it to print by default */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  if (!gstd_debug) {
    GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd", debug_color,
			     "GStraemer daemon debug category");

    debug_level = gst_debug_category_get_threshold (gstd_debug);
    if (debug_level < GSTD_DEBUG_DEFAULT_LEVEL)
      debug_level = GSTD_DEBUG_DEFAULT_LEVEL;
    
    gst_debug_set_threshold_for_name ("gstd", debug_level);
    GST_DEBUG ("Initialized gstd debug category");
  }
  
  /* Initialize the global pipeline list */
  if (!gstd_pipelines) {
    g_hash_table_new (g_str_hash, g_str_equal);
    GST_DEBUG ("Initialized pipelines hash table");
  }
}

void
gstd_deinit ()
{
  if (gstd_pipelines) {
    g_hash_table_unref (gstd_pipelines);
    gstd_pipelines = NULL;
    GST_DEBUG ("Deinitialized pipeline hash table");
  }

  if (gstd_debug) {
    GST_DEBUG ("Deinitialized gstd debug category");
    gst_debug_category_free (gstd_debug);
    gstd_debug = NULL;
  }
}

GstdReturnCode
gstd_create_pipeline (gchar *name, gchar *description, gchar **outname)
{
  GstdPipeline *gstd_pipeline;
  gint newindex;
  GError *error;
  
  if (!description)
    goto no_description;

  gstd_pipeline = gstd_pipeline_new();

  newindex = gstd_pipeline_get_highest_index (gstd_pipelines);
  newindex++;
  gstd_pipeline->index = newindex;
  
  if (!name || name[0] == '\0') {
    gstd_pipeline->name = g_strdup_printf("pipeline%d",newindex);
  } else {
    gstd_pipeline->name = g_strdup(name);
  }

  error = NULL;
  gstd_pipeline->pipeline = GST_PIPELINE(gst_parse_launch(description, &error));
  if (!gstd_pipeline->pipeline)
    goto wrong_pipeline;

  /* Still check if error is set because a recoverable
   * error might have occured */
  if (error) {
    GST_WARNING ("Recoverable error: %s", error->message);
    g_error_free (error);
  }

  if (g_hash_table_contains (gstd_pipelines, gstd_pipeline->name))
    goto existing_name;

  g_hash_table_insert (gstd_pipelines, gstd_pipeline->name, gstd_pipeline);

  return GSTD_EOK;
  
 no_description:
  {
    GST_ERROR ("Description must not be NULL");
    g_return_val_if_reached (GSTD_NULL_ARGUMENT);
  }
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
