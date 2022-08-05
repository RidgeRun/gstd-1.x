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
#include "libgstc_socket.h"
#include "libgstc_thread.h"

/* Test Fixture */
static gchar _request[3][512];
static GstClient *_client;
enum
{
  TEST_OK,
  TEST_TIMEOUT,
  TEST_CORRUPTED
};
static gint _status = TEST_OK;

static const char *_expected_response_ok = "{\n\
  \"code\" : 0,\n\
  \"description\" : \"Success\",\n\
  \"response\" : {\n\
    \"type\" : \"eos\",\n\
    \"source\" : \"pipe\",\n\
    \"timestamp\" : \"99:99:99.999999999\",\n\
    \"seqnum\" : 1276\n\
}\n\
}";
static const char *_expected_response_timeout = "{\n\
  \"code\" : 0,\n\
  \"description\" : \"Success\",\n\
  \"response\" : null\n\
}";

static const char *_expected_response_corrupted = "{\n\
  \"code\" : 0,\n\
  \"description\" : \"Success\",\n\
  \"resp\" : null\n\
}";

static void
setup (void)
{
  const gchar *address = "";
  unsigned int port = 0;
  unsigned long wait_time = 5;
  int keep_connection_open = 0;

  _status = TEST_OK;

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
  static int reqnum = 0;

  switch (_status) {
    case TEST_TIMEOUT:
      *response = malloc (strlen (_expected_response_timeout) + 1);
      memcpy (*response, _expected_response_timeout,
          strlen (_expected_response_timeout) + 1);
      break;
    case TEST_CORRUPTED:
      *response = malloc (strlen (_expected_response_corrupted) + 1);
      memcpy (*response, _expected_response_corrupted,
          strlen (_expected_response_corrupted) + 1);
      break;
    default:
      *response = malloc (strlen (_expected_response_ok) + 1);
      memcpy (*response, _expected_response_ok,
          strlen (_expected_response_ok) + 1);
      break;
  }

  memcpy (_request[reqnum], request, strlen (request));

  reqnum++;

  return GSTC_OK;
}

GST_START_TEST (test_pipeline_bus_wait_success)
{
  GstcStatus ret;
  gchar *message;
  const gchar *pipeline_name = "pipe";
  const gchar *message_name = "eos";
  const gint64 timeout = -1;
  const gchar *expected[] = { "update /pipelines/pipe/bus/types eos",
    "update /pipelines/pipe/bus/timeout -1",
    "read /pipelines/pipe/bus/message"
  };

  ret =
      gstc_pipeline_bus_wait (_client, pipeline_name, message_name,
      timeout, &message);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);
  assert_equals_string (expected[2], _request[2]);
  assert_equals_string (_expected_response_ok, message);

  g_free (message);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_bus_wait_timeout)
{
  GstcStatus ret;
  gchar *message;
  const gchar *pipeline_name = "pipe";
  const gchar *message_name = "eos";
  const gint64 timeout = -1;
  const gchar *expected[] = { "update /pipelines/pipe/bus/types eos",
    "update /pipelines/pipe/bus/timeout -1",
    "read /pipelines/pipe/bus/message"
  };

  _status = TEST_TIMEOUT;

  ret =
      gstc_pipeline_bus_wait (_client, pipeline_name, message_name,
      timeout, &message);
  assert_equals_int (GSTC_BUS_TIMEOUT, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);
  assert_equals_string (expected[2], _request[2]);
  assert_equals_string (_expected_response_timeout, message);

  g_free (message);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_bus_wait_corrupted)
{
  GstcStatus ret;
  gchar *message;
  const gchar *pipeline_name = "pipe";
  const gchar *message_name = "eos";
  const gint64 timeout = -1;
  const gchar *expected[] = { "update /pipelines/pipe/bus/types eos",
    "update /pipelines/pipe/bus/timeout -1",
    "read /pipelines/pipe/bus/message"
  };

  _status = TEST_CORRUPTED;

  ret =
      gstc_pipeline_bus_wait (_client, pipeline_name, message_name,
      timeout, &message);
  assert_equals_int (GSTC_NOT_FOUND, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);
  assert_equals_string (expected[2], _request[2]);
  assert_equals_string (_expected_response_corrupted, message);

  g_free (message);
}

GST_END_TEST;

static Suite *
libgstc_pipeline_bus_wait_suite (void)
{
  Suite *suite = suite_create ("libgstc_pipeline_bus_wait");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_bus_wait_success);
  tcase_add_test (tc, test_pipeline_bus_wait_timeout);
  tcase_add_test (tc, test_pipeline_bus_wait_corrupted);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline_bus_wait);
