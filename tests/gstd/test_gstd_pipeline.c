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

#define TEST_PIPE "fakesrc ! fakesink"

static void
test_set_up (gpointer fixture, gconstpointer data)
{
  gstd_pipeline_init ();
}

static void
test_tear_down (gpointer fixture, gconstpointer data)
{
  gstd_pipeline_deinit ();
}

static void
test_create_pipeline_null_name (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline (NULL, TEST_PIPE, &outpipe);

  g_assert_cmpint(GSTD_EOK, ==, ret); 
  g_assert_cmpstr("pipeline0", ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_create_pipeline_empty_name (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("", TEST_PIPE, &outpipe);

  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("pipeline0", ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_create_pipeline_custom_name (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("custom", TEST_PIPE, &outpipe);

  g_assert_cmpint(GSTD_EOK, ==, ret); 
  g_assert_cmpstr("custom", ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_create_pipeline_multiple (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("first", TEST_PIPE, NULL);
  g_assert_cmpint(GSTD_EOK, ==, ret);

  outpipe = NULL;
  ret = gstd_create_pipeline ("second", TEST_PIPE, &outpipe);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("second", ==, GSTD_PIPELINE_NAME(outpipe));

  outpipe = NULL;
  ret = gstd_create_pipeline (NULL, TEST_PIPE, &outpipe);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("pipeline2", ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_create_pipeline_bad_pipeline (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GstdReturnCode ret;
  guint size;

  ret = gstd_create_pipeline (NULL, "this_is_a_bad_pipeline", &outpipe);
  size = g_hash_table_size (gstd_pipeline_get_list());

  g_assert(!outpipe);
  g_assert_cmpint(GSTD_BAD_DESCRIPTION, ==, ret);
  g_assert_cmpuint(0, ==, size);
}

static void
test_destroy_pipeline_no_pipe (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;

  ret = gstd_pipeline_destroy("some_name");
  
  g_assert_cmpint(GSTD_NO_PIPELINE, ==, ret);
}

static void
test_destroy_pipeline_existing (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  guint size;

  ret = gstd_pipeline_create("pipe", TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);

  size = g_hash_table_size(gstd_pipeline_get_list());
  g_assert_cmpuint(1, ==, size);
  
  ret = gstd_pipeline_destroy("pipe");
  g_assert_cmpint(GSTD_EOK, ==, ret);

  size = g_hash_table_size(gstd_pipeline_get_list());
  g_assert_cmpuint(0, ==, size);
}

static void
test_destroy_pipeline_unexistent (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  guint size;
  
  ret = gstd_pipeline_destroy("pipe");
  g_assert_cmpint(GSTD_NO_PIPELINE, ==, ret);
}

static void
test_get_by_name_existing (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  GstdPipeline *outpipe = NULL;
  guint size;
  const gchar *name = "pipe";

  ret = gstd_pipeline_create(name, TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);

  ret = gstd_pipeline_get_by_name("pipe", &outpipe);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr(name, ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_get_by_name_unexistent (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  GstdPipeline *outpipe = NULL;
  guint size;
  const gchar *name = "pipe";

  ret = gstd_pipeline_get_by_name("pipe", &outpipe);
  g_assert_cmpint(GSTD_NO_PIPELINE, ==, ret);
  g_assert(!outpipe);
}

static void
test_get_by_index_existing (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  GstdPipeline *outpipe = NULL;

  ret = gstd_pipeline_create("first", TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);

  ret = gstd_pipeline_create("second", TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);

  ret = gstd_pipeline_get_by_index(0, &outpipe);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("first", ==, GSTD_PIPELINE_NAME(outpipe));

  ret = gstd_pipeline_get_by_index(1, &outpipe);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("second", ==, GSTD_PIPELINE_NAME(outpipe));
}

static void
test_get_by_index_unexistent (gpointer fixture, gconstpointer data)
{
  GstdReturnCode ret;
  gchar *outname = NULL;
  GstdPipeline *outpipe = NULL;
  guint size;
  const gchar *name = "pipe";

  ret = gstd_pipeline_get_by_index(0, &outpipe);
  g_assert_cmpint(GSTD_NO_PIPELINE, ==, ret);
  g_assert(!outpipe);
}

gint
main (gint argc, gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  gst_init (&argc, &argv);
  
  // Install the tests.
  g_test_add ("/gstd/gstd_pipeline/create_pipeline/null_name",
	      gpointer, NULL, test_set_up,
	      test_create_pipeline_null_name, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/create_pipeline/empty_name",
	      gpointer, NULL, test_set_up,
	      test_create_pipeline_empty_name, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/create_pipeline/custom_name",
	      gpointer, NULL, test_set_up,
	      test_create_pipeline_custom_name, test_tear_down);
  
  g_test_add ("/gstd/gstd_pipeline/create_pipeline/multiple",
	      gpointer, NULL, test_set_up,
	      test_create_pipeline_multiple, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/create_pipeline/bad_pipeline",
	      gpointer, NULL, test_set_up,
	      test_create_pipeline_bad_pipeline, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/destroy_pipeline/existing",
	      gpointer, NULL, test_set_up,
	      test_destroy_pipeline_existing, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/destroy_pipeline/unexistent",
	      gpointer, NULL, test_set_up,
	      test_destroy_pipeline_unexistent, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/get_by_name/existing",
	      gpointer, NULL, test_set_up,
	      test_get_by_name_existing, test_tear_down);

  g_test_add ("/gstd/gstd_pipeline/get_by_name/unexistent",
	      gpointer, NULL, test_set_up,
	      test_get_by_name_unexistent, test_tear_down);
  
  return g_test_run ();
}
