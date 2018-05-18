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

#include "libgstc_json.h"

/* Test Fixture */


GST_START_TEST (test_json_empty)
{
  GstcStatus ret;
  gint code;

  ret = gstc_json_get_int (NULL, "code", &code);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_int)
{
  GstcStatus ret;
  gint code;
  const char *json = "{ \"string\" : \"string val\", \"int\" : 100 }";

  ret = gstc_json_get_int (json, "int", &code);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (code, 100);
}

GST_END_TEST;

GST_START_TEST (test_json_int_neg)
{
  GstcStatus ret;
  gint code;
  const char *json = "{ \"string\" : \"string val\", \"int\" : -100 }";

  ret = gstc_json_get_int (json, "int", &code);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (code, -100);
}

GST_END_TEST;

GST_START_TEST (test_json_int_wrong_type)
{
  GstcStatus ret;
  gint code;
  const char * json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, "string", &code);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}
GST_END_TEST;

GST_START_TEST (test_json_int_null_name)
{
  GstcStatus ret;
  gint code;
  const char * json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, NULL, &code);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}
GST_END_TEST;

GST_START_TEST (test_json_int_null_placeholder)
{
  GstcStatus ret;
  const char * json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, "string", NULL);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}
GST_END_TEST;

GST_START_TEST (test_json_int_corrupted)
{
  GstcStatus ret;
  int out;
  const char * json = "{ \"int\" : 123 "; // Note the missing closing }

  ret = gstc_json_get_int (json, "int", &out);

  assert_equals_int (GSTC_MALFORMED, ret);
}
GST_END_TEST;

static Suite *
libgstc_ping_suite (void)
{
  Suite *suite = suite_create ("libgstc_json");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_test (tc, test_json_empty);
  tcase_add_test (tc, test_json_int);
  tcase_add_test (tc, test_json_int_neg);
  tcase_add_test (tc, test_json_int_wrong_type);
  tcase_add_test (tc, test_json_int_null_name);
  tcase_add_test (tc, test_json_int_null_placeholder);
  tcase_add_test (tc, test_json_int_corrupted);

  return suite;
}

GST_CHECK_MAIN (libgstc_ping);
