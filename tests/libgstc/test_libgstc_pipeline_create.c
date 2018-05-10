/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2018 Ridgerun, LLC (http://www.ridgerun.com)
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
#include <gst/check/gstcheck.h>
#include <string.h>

#include "libgstc.h"
#include "libgstc_socket.h"

/* Test Fixture */
static gchar _request[512];
static GstClient *_client;
static gboolean _reachable;

static void
setup ()
{
  const gchar * address = "";
  unsigned int port = 0;
  unsigned long wait_time = 0;
  int keep_connection_open = 0;

  _client = gstc_client_new (address, port, wait_time, keep_connection_open);
  _reachable = TRUE;
}

static void
teardown (void)
{
  gstc_client_free (_client);
}

/* Mock implementation of a socket */
typedef struct _GstcSocket
{
} GstcSocket;

GstcStatus
gstc_socket_send (GstcSocket *socket, const gchar *request)
{
  if (!_reachable) {
    return GSTC_UNREACHABLE;
  }

  memcpy (_request, request, strlen(request));

  return GSTC_OK;
}

GST_START_TEST (test_pipeline_create_success)
{
  GstcStatus ret;
  const gchar * pipeline_name = "pipe";
  const gchar * pipeline_desc = "fakesrc ! fakesink";
  const gchar * expected = "create /pipelines pipe fakesrc ! fakesink";

  ret = gstc_pipeline_create (_client, pipeline_name, pipeline_desc);
  fail_if (GSTC_OK != ret);

  assert_equals_string (expected, _request);
}
GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_name)
{
  GstcStatus ret;
  const gchar * pipeline_name = NULL;
  const gchar * pipeline_desc = "fakesrc ! fakesink";

  ret = gstc_pipeline_create (_client, pipeline_name, pipeline_desc);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}
GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_desc)
{
  GstcStatus ret;
  const gchar * pipeline_name = "pipe";
  const gchar * pipeline_desc = NULL;

  ret = gstc_pipeline_create (_client, pipeline_name, pipeline_desc);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}
GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_client)
{
  GstcStatus ret;
  const gchar * pipeline_name = "pipe";
  const gchar * pipeline_desc = "fakesrc ! fakesink";

  ret = gstc_pipeline_create (NULL, pipeline_name, pipeline_desc);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}
GST_END_TEST;

GST_START_TEST (test_pipeline_create_unreachable)
{
  GstcStatus ret;
  const gchar * pipeline_name = "pipe";
  const gchar * pipeline_desc = "fakesrc ! fakesink";

  _reachable = FALSE;

  ret = gstc_pipeline_create (_client, pipeline_name, pipeline_desc);
  assert_equals_int (GSTC_UNREACHABLE, ret);
}
GST_END_TEST;

static Suite *
libgstc_pipeline_suite (void)
{
  Suite *suite = suite_create ("libgstc_pipeline");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_create_success);
  tcase_add_test (tc, test_pipeline_create_null_name);
  tcase_add_test (tc, test_pipeline_create_null_desc);
  tcase_add_test (tc, test_pipeline_create_null_client);
  tcase_add_test (tc, test_pipeline_create_unreachable);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline);
