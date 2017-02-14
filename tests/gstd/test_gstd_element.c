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
test_get_list_existing (gpointer fixture, gconstpointer data)
{
  GstdPipeline *outpipe = NULL;
  GList *elements = NULL;
  GstdReturnCode ret;
  guint size;

  ret = gstd_pipeline_create (NULL, TEST_PIPE, &outpipe);
  g_assert_cmpint (ret, ==, GSTD_EOK);

  ret = gstd_element_get_list (outpipe, &elements);
  g_assert_cmpint (ret, ==, GSTD_EOK);

  size = g_list_length (elements);
  g_assert_cmpint (size, ==, 2);

  g_assert_cmpstr (GSTD_PIPELINE_NAME (GSTD_ELEMENT_PIPELINE ((GstdElement
                  *) (elements->data))), ==, outpipe->name);
  g_assert_cmpstr (GST_OBJECT_NAME (GSTD_ELEMENT_ELEMENT ((GstdElement
                  *) (elements->data))), ==, "fakesink0");
}

gint
main (gint argc, gchar * argv[])
{
  g_test_init (&argc, &argv, NULL);
  gst_init (&argc, &argv);

  // Install the tests.
  g_test_add ("/gstd/gstd_element/get_list/existing",
      gpointer, NULL, test_set_up, test_get_list_existing, test_tear_down);

  return g_test_run ();
}
