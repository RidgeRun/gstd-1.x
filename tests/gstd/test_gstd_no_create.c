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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/check/gstcheck.h>

#include "gstd_session.h"

const gchar *target_node[] = {
  "/pipelines/p0",
  "/pipelines/p0/elements",
  "/pipelines/p0/elements/fakesrc0",
  "/pipelines/p0/elements/fakesrc0/properties",
  "/pipelines/p0/elements/fakesrc0/properties/format",
  "/pipelines/p0/bus",
  "/pipelines/p0/bus/timeout",
  "/pipelines/p0/bus/types",
  "/pipelines/p0/name",
  "/pipelines/p0/description",
  "/pipelines/p0/state",
  "/pipelines/count",
  "/debug",
  "/debug/name",
  "/debug/enable",
  "/debug/color",
  "/debug/threshold",
  "/debug/flags"
};

GST_START_TEST (test_no_create)
{
  GstdObject *node;
  GstdReturnCode ret;
  GstdSession *test_session = gstd_session_new ("Test_session");
  gint i;

  /* Create pipeline to test no create cases */
  ret = gstd_get_by_uri (test_session, "/pipelines", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, "p0", "fakesrc ! fakesink");
  fail_if (ret);
  gst_object_unref (node);

  /* Tests */
  for (i = 0; i < sizeof (target_node) / sizeof (target_node[0]); i++) {
    ret = gstd_get_by_uri (test_session, target_node[i], &node);
    fail_if (ret);
    fail_if (NULL == node);

    ret = gstd_object_create (node, NULL, NULL);
    fail_if (GSTD_NO_CREATE != ret);
    gst_object_unref (node);
  }

  gst_object_unref (test_session);
}

GST_END_TEST;

static Suite *
gstd_no_create_suite (void)
{
  Suite *suite = suite_create ("gstd_no_create");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);
  tcase_add_test (tc, test_no_create);

  return suite;
}

GST_CHECK_MAIN (gstd_no_create);
