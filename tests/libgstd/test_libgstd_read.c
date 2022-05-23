/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
