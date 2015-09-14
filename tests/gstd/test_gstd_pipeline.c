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
test_create_pipeline_null_description (gpointer fixture, gconstpointer data)
{
  gchar *outname = NULL;
  
  gstd_pipeline_create ("name", NULL, &outname);
  g_test_trap_assert_failed();
}

static void
test_create_pipeline_null_name (gpointer fixture, gconstpointer data)
{
  gchar *outname = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline (NULL, TEST_PIPE, &outname);

  g_assert_cmpint(GSTD_EOK, ==, ret); 
  g_assert_cmpstr("pipeline0", ==, outname);
}

static void
test_create_pipeline_empty_name (gpointer fixture, gconstpointer data)
{
  gchar *outname = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("", TEST_PIPE, &outname);

  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("pipeline0", ==, outname);
}

static void
test_create_pipeline_custom_name (gpointer fixture, gconstpointer data)
{
  gchar *outname = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("custom", TEST_PIPE, &outname);

  g_assert_cmpint(GSTD_EOK, ==, ret); 
  g_assert_cmpstr("custom", ==, outname);
}

static void
test_create_pipeline_multiple (gpointer fixture, gconstpointer data)
{
  gchar *outname = NULL;
  GstdReturnCode ret;

  ret = gstd_create_pipeline ("first", TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("first", ==, outname);
  
  ret = gstd_create_pipeline ("second", TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("second", ==, outname);

  ret = gstd_create_pipeline (NULL, TEST_PIPE, &outname);
  g_assert_cmpint(GSTD_EOK, ==, ret);
  g_assert_cmpstr("pipeline2", ==, outname);
}

gint
main (gint argc, gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  gst_init (&argc, &argv);
  
  // Define the tests.
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
  
  return g_test_run ();
}
