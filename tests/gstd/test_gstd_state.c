/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/check/gstcheck.h>

#include "gstd_session.h"


GST_START_TEST (test_success)
{
  GstdObject *node;
  GstdReturnCode ret;
  GstdSession *test_session = gstd_session_new ("Test Session");

  /* Create pipeline to test no create cases */
  ret = gstd_get_by_uri (test_session, "/pipelines", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, "p0", "fakesrc ! fakesink");
  fail_if (ret);
  gst_object_unref (node);

  ret = gstd_get_by_uri (test_session, "/pipelines/p0/state", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_update (node, "playing");
  gst_object_unref (node);
  fail_if (ret);

  gst_object_unref (test_session);
}

GST_END_TEST;

GST_START_TEST (test_failure)
{
  GstdObject *node;
  GstdReturnCode ret;
  GstdSession *test_session = gstd_session_new ("Test Session");

  /* Create pipeline to test no create cases */
  ret = gstd_get_by_uri (test_session, "/pipelines", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, "p0", "fakesrc ! fakesink state-error=2");
  fail_if (ret);
  gst_object_unref (node);

  ret = gstd_get_by_uri (test_session, "/pipelines/p0/state", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_update (node, "playing");
  gst_object_unref (node);
  fail_if (ret != GSTD_STATE_ERROR);

  gst_object_unref (test_session);
}

GST_END_TEST;

static Suite *
gstd_state_suite (void)
{
  Suite *suite = suite_create ("gstd_state");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);
  tcase_add_test (tc, test_success);
  tcase_add_test (tc, test_failure);

  return suite;
}

GST_CHECK_MAIN (gstd_state);
