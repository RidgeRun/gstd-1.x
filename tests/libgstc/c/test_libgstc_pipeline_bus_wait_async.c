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
#include <time.h>
#include <unistd.h>

#include "libgstc.h"
#include "libgstc_socket.h"
#include "libgstc_thread.h"
#include "libgstc_assert.h"
#include "libgstc_json.h"

/* Test Fixture */
static gchar _request[3][512];
static GstClient *_client;
static GMutex lock;
int socket_send_wait_time = 0;

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

  *response = malloc (1);

  if (reqnum == 2) {
    sleep (socket_send_wait_time);
  }
  memcpy (_request[reqnum], request, strlen (request));

  reqnum++;


  return GSTC_OK;
}

GstcStatus
gstc_json_is_null (const gchar * json, const gchar * name, gint * out)
{
  *out = 0;
  return GSTC_OK;
}

GstcStatus
gstc_json_get_int (const gchar * json, const gchar * name, gint * out)
{
  return *out = GSTC_OK;
}

GstcStatus
gstc_json_get_child_char_array (const char *json, const char *parent_name,
    const char *array_name, const char *element_name, char **out[],
    int *array_lenght)
{
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

static GstcStatus
callback (GstClient * _client, const gchar * pipeline_name,
    const gchar * message_name, const long long timeout, char *message,
    gpointer user_data)
{
  /* Unlock the mutex */
  g_mutex_unlock (&lock);
  return GSTC_OK;
}

GST_START_TEST (test_pipeline_bus_wait_async_success)
{
  GstcStatus ret;
  const gchar *pipeline_name = "pipe";
  const gchar *message_name = "eos";
  const gint64 timeout = -1;
  const gchar *expected[] = { "update /pipelines/pipe/bus/types eos",
    "update /pipelines/pipe/bus/timeout -1",
    "read /pipelines/pipe/bus/message"
  };

  g_mutex_init (&lock);

  /*
   * Lock the mutex, this should be unlocked by the callback function
   */
  g_mutex_lock (&lock);

  ret =
      gstc_pipeline_bus_wait_async (_client, pipeline_name, message_name,
      timeout, callback, NULL);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);

  /* Wait for the callback function to finish or timeout passes */
  g_mutex_lock (&lock);
  assert_equals_string (expected[2], _request[2]);
  g_mutex_unlock (&lock);
}

GST_END_TEST;

static Suite *
libgstc_pipeline_bus_wait_async_suite (void)
{
  Suite *suite = suite_create ("libgstc_pipeline_bus_wait_async");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_bus_wait_async_success);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline_bus_wait_async);
