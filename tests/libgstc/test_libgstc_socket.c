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
#include <gio/gio.h>

#include "libgstc.h"
#include "libgstc_socket.h"

/* Mock implementation of gstd */
GMainLoop *_loop;
GSocketService *_mock_server;
GThread *_mock_thread;
const gchar *_mock_expected;

static gboolean
mock_server_cb (GSocketService * service, GSocketConnection * connection,
    GObject * source_object, gpointer user_data)
{
  GError *error;
  GInputStream *istream =
      g_io_stream_get_input_stream (G_IO_STREAM (connection));
  GOutputStream *ostream =
      g_io_stream_get_output_stream (G_IO_STREAM (connection));
  gchar message[64];
  gssize count;

  while (TRUE) {
    count = g_input_stream_read (istream, message, 1024, NULL, &error);
    fail_if (error);
    fail_if (-1 == count);

    if (0 == count) {
      break;
    }

    fail_if (NULL == _mock_expected);

    count = g_output_stream_write (ostream,
        _mock_expected, strlen (_mock_expected) + 1, NULL, &error);
    fail_if (error);
    fail_if (-1 == count);
  }

  return TRUE;
}

gpointer
mock_server_thread (gpointer data)
{
  g_main_loop_run (_loop);

  return NULL;
}

static void
mock_server_new ()
{
  GError *error = NULL;
  gint64 start_time;
  gint64 start_timeout = 1000000;       /* 1s */
  gint64 loop_wait_time = 5000;        /* 5ms */
  gint64 time_passed = 0;

  _mock_server = g_threaded_socket_service_new (-1);
  _mock_expected = NULL;

  g_socket_listener_add_inet_port ((GSocketListener *) _mock_server,
      54321, NULL, &error);
  fail_if (error);

  g_signal_connect (_mock_server, "run", G_CALLBACK (mock_server_cb), NULL);
  g_socket_service_start (_mock_server);

  _loop = g_main_loop_new (NULL, FALSE);
  _mock_thread = g_thread_new ("mock_server", mock_server_thread, NULL);

  /* 
   * FIXME this is done in order to ensure that the socket loop is running
   * before starting the test and isn't dereferenced in an inconsistent state.
   * This timeouts after 1s
   */
  start_time = g_get_monotonic_time ();
  while (!g_main_loop_is_running (_loop) ) {
    g_usleep (loop_wait_time);  /* Wait before checking again */
      
    /* Exit if timeout is reached */
    time_passed = g_get_monotonic_time () - start_time;
    g_return_if_fail(time_passed < start_timeout);
  }
}

static void
mock_server_free ()
{
  g_socket_service_stop (_mock_server);
  g_object_unref (_mock_server);

  g_main_loop_quit (_loop);
  g_thread_join (_mock_thread);
  g_main_loop_unref (_loop);
  g_thread_unref (_mock_thread);
}

/* Mock implementation of malloc, replaced in Makefile.am */
static gboolean _mock_malloc_oom;

gpointer
mock_malloc (gsize size)
{
  if (_mock_malloc_oom) {
    return NULL;
  } else {
    return g_malloc (size);
  }
}

void
setup ()
{
  _mock_malloc_oom = FALSE;
  mock_server_new ();
}

void
teardown ()
{
  mock_server_free ();
}

GST_START_TEST (test_socket_success)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;
  const gchar *request = "ping";
  const gchar *expected = "pong";
  gchar *response;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_OK, ret);
  fail_if (NULL == socket);

  _mock_expected = expected;
  ret = gstc_socket_send (socket, request, &response);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_string (expected, response);

  g_free (response);
  gstc_socket_free (socket);
}

GST_END_TEST;

GST_START_TEST (test_socket_persistent)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;
  const gchar *request = "ping";
  const gchar *expected = "pong";
  gchar *response;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_OK, ret);
  fail_if (NULL == socket);

  _mock_expected = expected;
  ret = gstc_socket_send (socket, request, &response);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_string (expected, response);

  g_free (response);
  _mock_expected = expected = "ping";
  request = "ping";
  ret = gstc_socket_send (socket, request, &response);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_string (expected, response);

  g_free (response);
  gstc_socket_free (socket);
}

GST_END_TEST;

GST_START_TEST (test_socket_oom)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;

  _mock_malloc_oom = TRUE;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_OOM, ret);
  assert_equals_pointer (NULL, socket);
}

GST_END_TEST;

GST_START_TEST (test_socket_unreachable)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "500.0.0.1";   /* Note the invalid IP */
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_UNREACHABLE, ret);
  assert_equals_pointer (NULL, socket);
}

GST_END_TEST;

GST_START_TEST (test_socket_null_address)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = NULL;
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_socket_null_placeholder)
{
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;

  ret = gstc_socket_new (address, port, wait_time, keep_open, NULL);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_socket_null_socket)
{
  GstcStatus ret;
  const gchar *request = "ping";
  gchar *response;

  ret = gstc_socket_send (NULL, request, &response);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_socket_null_request)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;
  const gchar *request = NULL;
  gchar *response;

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_OK, ret);

  ret = gstc_socket_send (socket, request, &response);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);

  gstc_socket_free (socket);
}

GST_END_TEST;

GST_START_TEST (test_socket_null_resp_placeholder)
{
  GstcSocket *socket;
  GstcStatus ret;
  const gchar *address = "127.0.0.1";
  const gint port = 54321;
  const unsigned long wait_time = 0;
  const gint keep_open = TRUE;
  const gchar *request = "ping";

  ret = gstc_socket_new (address, port, wait_time, keep_open, &socket);
  assert_equals_int (GSTC_OK, ret);

  ret = gstc_socket_send (socket, request, NULL);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);

  gstc_socket_free (socket);
}

GST_END_TEST;

static Suite *
libgstc_client_suite (void)
{
  Suite *suite = suite_create ("libgstc_socket");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_socket_success);
  tcase_add_test (tc, test_socket_persistent);
  tcase_add_test (tc, test_socket_oom);
  tcase_add_test (tc, test_socket_unreachable);
  tcase_add_test (tc, test_socket_null_address);
  tcase_add_test (tc, test_socket_null_placeholder);
  tcase_add_test (tc, test_socket_null_socket);
  tcase_add_test (tc, test_socket_null_request);
  tcase_add_test (tc, test_socket_null_resp_placeholder);

  return suite;
}

GST_CHECK_MAIN (libgstc_client);
