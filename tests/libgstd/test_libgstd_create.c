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
static const gchar *description = "videotestsrc ! fakesink";

static void
setup (void)
{
  gstd_new (&manager, 0, NULL);
}

static void
teardown (void)
{
  gstd_free (manager);
}

GST_START_TEST (test_pipeline_create_successful)
{
  GstdReturnCode ret = GSTD_EOK;
  ret = gstd_create (manager, uri, pipe_name, description);
  fail_if (GSTD_EOK != ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_create_bad_uri)
{
  GstdReturnCode ret = GSTD_EOK;
  ret = gstd_create (manager, "/qwerty", pipe_name, description);

  fail_if (GSTD_BAD_COMMAND != ret);
}

GST_END_TEST;


GST_START_TEST (test_pipeline_create_existing_resource)
{
  GstdReturnCode ret = GSTD_EOK;

  ret = gstd_create (manager, uri, "pipe_name", description);
  fail_if (GSTD_EOK != ret);

  ret = gstd_create (manager, uri, "pipe_name", description);
  fail_if (GSTD_EXISTING_RESOURCE != ret);
}

GST_END_TEST;


GST_START_TEST (test_pipeline_create_bad_desc)
{
  GstdReturnCode ret = GSTD_EOK;
  ret = gstd_create (manager, uri, pipe_name, "description");

  fail_if (GSTD_BAD_DESCRIPTION != ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_needed_desc)
{
  GstdReturnCode ret = GSTD_EOK;
  ret = gstd_create (manager, uri, pipe_name, NULL);

  fail_if (GSTD_MISSING_ARGUMENT != ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_create_no_needed_desc)
{
  GstdReturnCode ret = GSTD_EOK;
  ret = gstd_create (manager, "/pipelines/p1/event", "eos", description);

  fail_if (GSTD_BAD_COMMAND != ret);
}

GST_END_TEST;

static Suite *
gstd_create_suite (void)
{
  Suite *suite = suite_create ("gstd_create");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_create_successful);
  tcase_add_test (tc, test_pipeline_create_bad_uri);
  tcase_add_test (tc, test_pipeline_create_existing_resource);
  tcase_add_test (tc, test_pipeline_create_bad_desc);
  tcase_add_test (tc, test_pipeline_create_null_needed_desc);
  tcase_add_test (tc, test_pipeline_create_no_needed_desc);

  return suite;
}

GST_CHECK_MAIN (gstd_create);
