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
#include "libgstc_assert.h"

/* Test Fixture */
static gchar _request[512];
static GstClient *_client;

static void
setup ()
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

  memcpy (_request, request, strlen (request));

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
  gstc_assert_and_ret_val (NULL != array_name, GSTC_NULL_ARGUMENT);

  return GSTC_OK;
}

GST_START_TEST (test_pipeline_list_properties_success)
{
  GstcStatus ret;
  char *expected = "read /pipelines/pipe/elements/element/properties";
  char *pipeline_name = "pipe";
  char *element_name = "element";
  char **response;
  int array_lenght;

  ret =
      gstc_element_properties_list (_client, pipeline_name, element_name,
      &response, &array_lenght);

  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected, _request);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_list_properties_null_list_lenght)
{
  GstcStatus ret;
  char **response;
  char *pipeline_name = "pipe";
  char *element_name = "element";
  int *array_lenght = NULL;

  ret =
      gstc_element_properties_list (_client, pipeline_name, element_name,
      &response, array_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_list_properties_null_client)
{
  GstcStatus ret;
  char **response;
  char *pipeline_name = "pipe";
  char *element_name = "element";
  int array_lenght;

  ret =
      gstc_element_properties_list (NULL, pipeline_name, element_name,
      &response, &array_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_list_properties_null_pipeline_name)
{
  GstcStatus ret;
  char **response;
  char *pipeline_name = NULL;
  char *element_name = "element";
  int array_lenght;

  ret =
      gstc_element_properties_list (_client, pipeline_name, element_name,
      &response, &array_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_list_properties_null_element_name)
{
  GstcStatus ret;
  char **response;
  char *pipeline_name = "pipe";
  char *element_name = NULL;
  int array_lenght;

  ret =
      gstc_element_properties_list (_client, pipeline_name, element_name,
      &response, &array_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

static Suite *
libgstc_pipeline_list_properties_suite (void)
{
  Suite *suite = suite_create ("libgstc_pipeline_list_properties");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_checked_fixture (tc, setup, teardown);
  tcase_add_test (tc, test_pipeline_list_properties_success);
  tcase_add_test (tc, test_pipeline_list_properties_null_list_lenght);
  tcase_add_test (tc, test_pipeline_list_properties_null_client);
  tcase_add_test (tc, test_pipeline_list_properties_null_pipeline_name);
  tcase_add_test (tc, test_pipeline_list_properties_null_element_name);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline_list_properties);
