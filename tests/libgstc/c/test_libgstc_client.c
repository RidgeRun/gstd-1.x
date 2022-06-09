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

#include "libgstc.h"
#include "libgstc_socket.h"
#include "libgstc_assert.h"
#include "libgstc_json.h"

/* Mock implementation of a socket */
typedef struct _GstcSocket
{
} GstcSocket;

static GstcSocket _socket;
static gboolean _fail_socket = FALSE;

GstcStatus
gstc_socket_new (const char *address, const unsigned int port,
    const int keep_connection_open, GstcSocket ** out)
{
  if (_fail_socket) {
    *out = NULL;
    return GSTC_SOCKET_ERROR;
  } else {
    *out = &_socket;
    return GSTC_OK;
  }
}

void
gstc_socket_free (GstcSocket * socket)
{
}

GstcStatus
gstc_socket_send (GstcSocket * socket, const gchar * request, gchar ** response,
    const int timeout)
{
  *response = malloc (1);

  return GSTC_OK;
}

GstcStatus
gstc_json_get_int (const gchar * json, const gchar * name, gint * out)
{
  return *out = GSTC_OK;
}

GstcStatus
gstc_json_is_null (const gchar * json, const gchar * name, gint * out)
{
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

/* Mock implementation of malloc */
gboolean _use_mock_malloc = FALSE;
void *
mock_malloc (size_t size)
{
  if (_use_mock_malloc) {
    /* Simulate out of memory */
    return NULL;
  } else {
    return calloc (1, size);
  }
}

static void
setup (void)
{
  _use_mock_malloc = FALSE;
  _fail_socket = FALSE;
}

static void
teardown (void)
{
}

GST_START_TEST (test_client_success)
{
  GstClient *client;
  GstcStatus ret;

  const gchar *address = "127.0.0.1";
  guint port = 12345;
  guint64 wait_time = 0;
  gint keep_connection_open = 1;

  ret =
      gstc_client_new (address, port, wait_time, keep_connection_open, &client);
  assert_equals_int (GSTC_OK, ret);
  fail_if (NULL == client);

  gstc_client_free (client);
}

GST_END_TEST;

GST_START_TEST (test_client_out_of_memory)
{
  GstClient *client;
  GstcStatus ret;

  const gchar *address = "127.0.0.1";
  guint port = 12345;
  guint64 wait_time = 0;
  gint keep_connection_open = 1;

  _use_mock_malloc = TRUE;

  ret =
      gstc_client_new (address, port, wait_time, keep_connection_open, &client);
  assert_equals_int (GSTC_OOM, ret);
  assert_equals_pointer (NULL, client)
}

GST_END_TEST;

GST_START_TEST (test_client_null_address)
{
  GstClient *client;
  GstcStatus ret;

  const gchar *address = NULL;
  guint port = 12345;
  guint64 wait_time = 0;
  gint keep_connection_open = 1;

  ret =
      gstc_client_new (address, port, wait_time, keep_connection_open, &client);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_client_null_placeholder)
{
  GstcStatus ret;

  const gchar *address = "127.0.0.1";
  guint port = 12345;
  guint64 wait_time = 0;
  gint keep_connection_open = 1;

  ret = gstc_client_new (address, port, wait_time, keep_connection_open, NULL);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_client_null_in_free)
{
  gstc_client_free (NULL);
}

GST_END_TEST;

GST_START_TEST (test_client_no_socket)
{
  GstClient *client;
  GstcStatus ret;

  const gchar *address = "127.0.0.1";
  guint port = 12345;
  guint64 wait_time = 0;
  gint keep_connection_open = 1;

  _fail_socket = TRUE;

  ret =
      gstc_client_new (address, port, wait_time, keep_connection_open, &client);
  fail_if (GSTC_OK, ret);
  assert_equals_pointer (NULL, client);
}

GST_END_TEST;

static Suite *
libgstc_client_suite (void)
{
  Suite *suite = suite_create ("libgstc_client");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_client_success);
  tcase_add_test (tc, test_client_out_of_memory);
  tcase_add_test (tc, test_client_null_address);
  tcase_add_test (tc, test_client_null_placeholder);
  tcase_add_test (tc, test_client_null_in_free);
  tcase_add_test (tc, test_client_no_socket);

  return suite;
}

GST_CHECK_MAIN (libgstc_client);
