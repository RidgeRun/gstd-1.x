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
#include <string.h>

#include "libgstc.h"
#include "libgstc_json.h"
#include "libgstc_socket.h"
#include "libgstc_assert.h"

/* Test Fixture */
static gchar _request[4][512];
static GstClient *_client;
int request_count = 0;

static void
setup (void)
{
  const gchar *address = "";
  unsigned int port = 0;
  unsigned long wait_time = 5;
  int keep_connection_open = 0;

  gstc_client_new (address, port, wait_time, keep_connection_open, &_client);
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

GstcSocket _socket;

GstcStatus
gstc_socket_new (const char *address, const unsigned int port,
    const int keep_connection_open, GstcSocket ** out)
{
  gstc_assert_and_ret_val (NULL != address, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  *out = &_socket;

  return GSTC_OK;
}

void
gstc_socket_free (GstcSocket * socket)
{
}

GstcStatus
gstc_socket_send (GstcSocket * socket, const gchar * request, gchar ** response,
    const int timeout)
{
  gstc_assert_and_ret_val (NULL != socket, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != request, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != response, GSTC_NULL_ARGUMENT);

  *response = malloc (1);

  memcpy (_request[request_count], request, strlen (request));
  request_count++;

  return GSTC_OK;
}

GstcStatus
gstc_json_get_int (const gchar * json, const gchar * name, gint * out)
{
  gstc_assert_and_ret_val (NULL != json, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  return *out = GSTC_OK;
}

GstcStatus
gstc_json_is_null (const gchar * json, const gchar * name, gint * out)
{
  gstc_assert_and_ret_val (NULL != json, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  *out = 0;
  return GSTC_OK;
}

GstcStatus
gstc_json_get_child_char_array (const char *json, const char *parent_name,
    const char *array_name, const char *element_name, char **out[],
    int *array_lenght)
{
  gstc_assert_and_ret_val (NULL != json, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != parent_name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != array_name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != element_name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != array_lenght, GSTC_NULL_ARGUMENT);

  return GSTC_OK;
}

GstcStatus
gstc_json_child_string (const char *json, const char *parent_name,
    const char *data_name, char **out)
{
  gstc_assert_and_ret_val (NULL != json, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != parent_name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != data_name, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  return GSTC_OK;
}

GST_START_TEST (test_debug_success_false_color)
{
  GstcStatus ret;
  const char *threshold = "*:2";
  const int color_enable = 0;
  const int reset = 1;
  const gchar *expectedEnable = "update /debug/enable true";
  const gchar *expectedThreshold = "update /debug/threshold *:2";
  const gchar *expectedColor = "update /debug/color false";
  const gchar *expectedReset = "update /debug/reset true";

  ret = gstc_client_debug (_client, threshold, color_enable, reset);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expectedEnable, _request[0]);
  assert_equals_string (expectedThreshold, _request[1]);
  assert_equals_string (expectedColor, _request[2]);
  assert_equals_string (expectedReset, _request[3]);
}

GST_END_TEST;

GST_START_TEST (test_debug_success_true_color)
{
  GstcStatus ret;
  const char *threshold = "*:2";
  const int color_enable = 1;
  const int reset = 1;
  const gchar *expectedEnable = "update /debug/enable true";
  const gchar *expectedThreshold = "update /debug/threshold *:2";
  const gchar *expectedColor = "update /debug/color true";
  const gchar *expectedReset = "update /debug/reset true";

  ret = gstc_client_debug (_client, threshold, color_enable, reset);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expectedEnable, _request[0]);
  assert_equals_string (expectedThreshold, _request[1]);
  assert_equals_string (expectedColor, _request[2]);
  assert_equals_string (expectedReset, _request[3]);
}

GST_END_TEST;

GST_START_TEST (test_null_client)
{
  GstcStatus ret;
  const char *threshold = "*:2";
  const int color_enable = 0;
  const int reset = 1;

  ret = gstc_client_debug (NULL, threshold, color_enable, reset);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_null_threshold)
{
  GstcStatus ret;
  const char *threshold = NULL;
  const int color_enable = 0;
  const int reset = 1;

  ret = gstc_client_debug (_client, threshold, color_enable, reset);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

static Suite *
libgstc_pipeline_seek_suite (void)
{
  Suite *suite = suite_create ("libgstc_pipeline_seek");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_debug_success_false_color);
  tcase_add_test (tc, test_debug_success_true_color);
  tcase_add_test (tc, test_null_client);
  tcase_add_test (tc, test_null_threshold);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline_seek);
