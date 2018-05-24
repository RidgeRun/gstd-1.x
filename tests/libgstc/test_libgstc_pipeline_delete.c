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
    const unsigned long wait_time, const int keep_connection_open,
    GstcSocket ** out)
{
  *out = &_socket;

  return GSTC_OK;
}

void
gstc_socket_free (GstcSocket * socket)
{
}

GstcStatus
gstc_socket_send (GstcSocket * socket, const gchar * request, gchar ** response)
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

GST_START_TEST (test_pipeline_create_success)
{
  GstcStatus ret;
  const gchar *pipeline_name = "pipe";
  const gchar *expected = "delete /pipelines pipe";

  ret = gstc_pipeline_delete (_client, pipeline_name);
  assert_equals_int (GSTC_OK, ret);

  assert_equals_string (expected, _request);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_name)
{
  GstcStatus ret;
  const gchar *pipeline_name = NULL;

  ret = gstc_pipeline_delete (_client, pipeline_name);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_pipeline_create_null_client)
{
  GstcStatus ret;
  const gchar *pipeline_name = "pipe";

  ret = gstc_pipeline_delete (NULL, pipeline_name);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
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
  tcase_add_test (tc, test_pipeline_create_null_client);

  return suite;
}

GST_CHECK_MAIN (libgstc_pipeline);
