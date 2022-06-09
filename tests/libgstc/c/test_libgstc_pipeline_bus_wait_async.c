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
#include <pthread.h>
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
pthread_mutex_t lock;
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
  pthread_mutex_unlock (&lock);
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

  /* Mutex timeout 10ms */
  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);
  ts.tv_nsec += 10 * 1000000;

  if (pthread_mutex_init (&lock, NULL) != 0) {
    /* Exit if unable to create mutex lock */
    ASSERT_CRITICAL ();
  }

  /*
   * Lock the mutex, this should be unlocked by the callback function
   */
  pthread_mutex_lock (&lock);

  ret =
      gstc_pipeline_bus_wait_async (_client, pipeline_name, message_name,
      timeout, callback, NULL);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);

  /* Wait for the callback function to finish or timeout passes */
  pthread_mutex_timedlock (&lock, &ts);
  assert_equals_string (expected[2], _request[2]);
  pthread_mutex_unlock (&lock);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_bus_wait_async_bus_didnt_respond)
{
  GstcStatus ret;
  const gchar *pipeline_name = "pipe";
  const gchar *message_name = "eos";
  const gint64 timeout = -1;
  const gchar *expected[] = { "update /pipelines/pipe/bus/types eos",
    "update /pipelines/pipe/bus/timeout -1"
  };
  struct timespec ts;
  /* Set the send command to wait for 1s before responding */
  socket_send_wait_time = 1;

  /* Mutex timeout 10ms */
  clock_gettime (CLOCK_REALTIME, &ts);
  ts.tv_sec += 0;
  ts.tv_nsec += 10 * 1000000;

  if (pthread_mutex_init (&lock, NULL) != 0) {
    /* Exit if unable to create mutex lock */
    ASSERT_CRITICAL ();
  }

  /*
   * Lock the mutex, this should be unlocked by the callback function
   */
  pthread_mutex_lock (&lock);

  ret =
      gstc_pipeline_bus_wait_async (_client, pipeline_name, message_name,
      timeout, callback, NULL);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected[0], _request[0]);
  assert_equals_string (expected[1], _request[1]);

  /* Wait for the callback function to finish or timeout passes */
  pthread_mutex_timedlock (&lock, &ts);
  assert_equals_string ("", _request[2]);
  pthread_mutex_unlock (&lock);

  /* Reset value */
  socket_send_wait_time = 0;
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
  tcase_add_test (tc, test_pipeline_bus_wait_async_bus_didnt_respond);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline_bus_wait_async);
