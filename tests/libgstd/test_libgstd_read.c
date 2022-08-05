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

GST_START_TEST (test_pipeline_read_successful)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *resource = NULL;

  ret = gstd_read (manager, uri, &resource);
  fail_if (GSTD_EOK != ret);
  fail_if (NULL == resource);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_read_by_name_successful)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *resource = NULL;

  ret = gstd_read (manager, "pipelines/p1/elements/vts", &resource);
  fail_if (GSTD_EOK != ret);
  fail_if (NULL == resource);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_read_bad_uri)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *resource = NULL;
  ret = gstd_read (manager, "uri", &resource);

  fail_if (GSTD_BAD_COMMAND != ret);
  fail_if (NULL != resource);
}

GST_END_TEST;

static Suite *
gstd_read_suite (void)
{
  Suite *suite = suite_create ("gstd_read");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_read_successful);
  tcase_add_test (tc, test_pipeline_read_by_name_successful);
  tcase_add_test (tc, test_pipeline_read_bad_uri);

  return suite;
}

GST_CHECK_MAIN (gstd_read);
