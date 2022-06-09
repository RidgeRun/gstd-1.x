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
#include <string.h>

#include "libgstc.h"
#include "libgstc_json.h"
#include "libgstc_socket.h"
#include "libgstc_assert.h"

/* Test Fixture */
static gchar _request[512];
static GstClient *_client;
static int json_case = 0;

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
  *response = malloc (1);

  memcpy (_request, request, strlen (request));

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
  switch (json_case) {
    case 0:
      *out = malloc (2);
      (*out)[0] = '5';
      (*out)[1] = '\0';
      break;
    case 1:
      *out = malloc (2);
      (*out)[0] = 'a';
      (*out)[1] = '\0';
      break;
    case 2:
      *out = malloc (4);
      (*out)[0] = 'a';
      (*out)[1] = ' ';
      (*out)[2] = '5';
      (*out)[3] = '\0';
      break;
  }
  return GSTC_OK;
}

GST_START_TEST (test_property_get_int)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  const gchar *expected = "read /pipelines/pipe/elements/elem/properties/prop";
  int integer = -1;

  json_case = 0;
  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%d",
      &integer);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected, _request);

  assert_equals_int (5, integer);
}

GST_END_TEST;

GST_START_TEST (test_property_get_string)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  const gchar *expected = "read /pipelines/pipe/elements/elem/properties/prop";
  char string[64];

  json_case = 1;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%s", string);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected, _request);

  assert_equals_string ("a", string);
}

GST_END_TEST;

GST_START_TEST (test_property_get_combined)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  const gchar *expected = "read /pipelines/pipe/elements/elem/properties/prop";
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%s %d",
      string, &integer);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected, _request);

  assert_equals_string ("a", string);
  assert_equals_int (5, integer);
}

GST_END_TEST;

GST_START_TEST (test_property_get_null_client)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (NULL, pipe_name, elem_name, prop_name, "%s %d", string,
      &integer);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_property_get_null_pname)
{
  GstcStatus ret;
  const gchar *pipe_name = NULL;
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%s %d",
      string, &integer);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_property_get_null_element)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = NULL;
  const gchar *prop_name = "prop";
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%s %d",
      string, &integer);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_property_get_null_parameter)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = NULL;
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, "%s %d",
      string, &integer);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_property_get_null_format)
{
  GstcStatus ret;
  const gchar *pipe_name = "pipe";
  const gchar *elem_name = "elem";
  const gchar *prop_name = "prop";
  char string[64];
  int integer = -1;

  json_case = 2;

  ret =
      gstc_element_get (_client, pipe_name, elem_name, prop_name, NULL, string,
      &integer);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

static Suite *
libgstc_element_get_suite (void)
{
  Suite *suite = suite_create ("libgstc_element_get");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_property_get_int);
  tcase_add_test (tc, test_property_get_string);
  tcase_add_test (tc, test_property_get_combined);
  tcase_add_test (tc, test_property_get_null_client);
  tcase_add_test (tc, test_property_get_null_pname);
  tcase_add_test (tc, test_property_get_null_element);
  tcase_add_test (tc, test_property_get_null_parameter);
  tcase_add_test (tc, test_property_get_null_format);

  return suite;
}

GST_CHECK_MAIN (libgstc_element_get);
