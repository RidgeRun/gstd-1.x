/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/check/gstcheck.h>

#include "gstd.h"

static GstD *manager;
static const gchar *uri = "/pipelines";
static const gchar *pipe_name = "p1";
static const gchar *description = "videotestsrc name=vts ! fakesink";

static void
setup (void)
{
  gstd_new (&manager, 0, NULL);
  gstd_create (manager, uri, pipe_name, description);
}

static void
teardown (void)
{
  gstd_free (manager);
}

GST_START_TEST (test_pipeline_update_successful)
{
  GstdReturnCode ret = GSTD_EOK;
  gchar *uri_update = NULL;

  uri_update =
      g_strdup_printf ("/pipelines/%s/elements/vts/properties/pattern",
      pipe_name);
  ret = gstd_update (manager, uri_update, "1");

  fail_if (GSTD_EOK != ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_update_bad_uri)
{
  GstdReturnCode ret = GSTD_EOK;

  ret = gstd_update (manager, "uri", "1");

  fail_if (GSTD_BAD_COMMAND != ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_update_bad_value)
{
  GstdReturnCode ret = GSTD_EOK;
  gchar *uri_update = NULL;

  uri_update =
      g_strdup_printf ("/pipelines/%s/elements/vts/properties/pattern",
      pipe_name);
  ret = gstd_update (manager, uri_update, "value");

  fail_if (GSTD_BAD_VALUE != ret);
}

GST_END_TEST;

static Suite *
gstd_update_suite (void)
{
  Suite *suite = suite_create ("gstd_update");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_update_successful);
  tcase_add_test (tc, test_pipeline_update_bad_uri);
  tcase_add_test (tc, test_pipeline_update_bad_value);

  return suite;
}

GST_CHECK_MAIN (gstd_update);
