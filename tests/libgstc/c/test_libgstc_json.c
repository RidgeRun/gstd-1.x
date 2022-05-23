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

GST_START_TEST (test_json_is_null)
{
  GstcStatus ret;
  gint is_null;
  const char *json = "{ \"string\" : \"string val\", \"null\" : null }";

  ret = gstc_json_is_null (json, "null", &is_null);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (is_null, 1);
}

GST_END_TEST;

GST_START_TEST (test_json_is_not_null)
{
  GstcStatus ret;
  gint is_null;
  const char *json = "{ \"string\" : \"string val\", \"null\" : \"not null\" }";

  ret = gstc_json_is_null (json, "null", &is_null);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (is_null, 0);
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
  const char *json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, "string", &code);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_int_null_name)
{
  GstcStatus ret;
  gint code;
  const char *json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, NULL, &code);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_int_null_placeholder)
{
  GstcStatus ret;
  const char *json = "{ \"string\" : \"string val\" }";

  ret = gstc_json_get_int (json, "string", NULL);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_int_corrupted)
{
  GstcStatus ret;
  int out;
  const char *json = "{ \"int\" : 123 ";        // Note the missing closing }

  ret = gstc_json_get_int (json, "int", &out);

  assert_equals_int (GSTC_MALFORMED, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_int_missing)
{
  GstcStatus ret;
  int out;
  const char *json = "{ \"int\" : 123 }";

  ret = gstc_json_get_int (json, "integer", &out);

  assert_equals_int (GSTC_NOT_FOUND, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_success)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (3, list_lenght);

  assert_equals_string (result[0], "result1");
  assert_equals_string (result[1], "result2");
  assert_equals_string (result[2], "result3");
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_sucess)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, "parent", "value_name", &out);

  assert_equals_int (GSTC_OK, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_empty_array)
{
  GstcStatus ret;
  const char *json = "{ \"parent\" : { \"array\" : [] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "name", &result, &list_lenght);

  assert_equals_int (GSTC_OK, ret);
  assert_equals_int (0, list_lenght);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_null_json)
{
  GstcStatus ret;
  char *out;

  ret = gstc_json_child_string (NULL, "parent", "value_name", &out);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_json)
{
  GstcStatus ret;
  const char *json = NULL;
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_null_parent)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, NULL, "value_name", &out);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_parent_name)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, NULL, "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_null_name)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, "parent", NULL, &out);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_array_name)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", NULL,
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_null_out)
{
  GstcStatus ret;
  char **out = NULL;
  const char *json = "{ \"parent\" :  {  \"value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, "parent", "value_name", out);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_element_name)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      NULL, &result, &list_lenght);
  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_out)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int list_lenght;
  char ***result = NULL;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", result, &list_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_invalid_json)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"value_name\" : \"value\" }  ";

  ret = gstc_json_child_string (json, "parent", "value_name", &out);

  assert_equals_int (GSTC_MALFORMED, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_null_list_lenght)
{
  GstcStatus ret;
  const char *json =
      "{ \"parent\" : { \"array\" : [{ \"string\" : \"result1\" }, \
    { \"string\" : \"result2\" },{ \"string\" : \"result3\" }] } } ";
  int *list_lenght = NULL;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, list_lenght);

  assert_equals_int (GSTC_NULL_ARGUMENT, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_arrays_parent_wrong_type)
{
  GstcStatus ret;
  /* In this case parent is int not object */
  const char *json = "{ \"parent\" : 15 }";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_missing_parent)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"not_parent\" :  {  \"value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, "parent", "value_name", &out);

  assert_equals_int (GSTC_NOT_FOUND, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_array_wrong_type)
{
  GstcStatus ret;
  /* In this case array is int not array */
  const char *json = "{ \"parent\" : { \"array\" : 15 } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_arrays_child_wrong_type)
{
  GstcStatus ret;
  /* Array's elements must be objects */
  const char *json = "{ \"parent\" : { \"array\" : [ 5, \"string\", 7 ] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_missing_object)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"not_value_name\" : \"value\" }  }";

  ret = gstc_json_child_string (json, "parent", "value_name", &out);

  assert_equals_int (GSTC_NOT_FOUND, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_char_array_arrays_child_not_string)
{
  GstcStatus ret;
  /* Array's elements must contain strings */
  const char *json = "{ \"parent\" : { \"array\" : [{ \"string\" : 5 }] } } ";
  int list_lenght;
  char **result;

  ret = gstc_json_get_child_char_array (json, "parent", "array",
      "string", &result, &list_lenght);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

GST_START_TEST (test_json_child_string_wrong_type)
{
  GstcStatus ret;
  char *out;
  const char *json = "{ \"parent\" :  {  \"value_name\" : 0 }  }";

  ret = gstc_json_child_string (json, "parent", "value_name", &out);

  assert_equals_int (GSTC_TYPE_ERROR, ret);
}

GST_END_TEST;

static Suite *
libgstc_ping_suite (void)
{
  Suite *suite = suite_create ("libgstc_json");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_test (tc, test_json_empty);
  tcase_add_test (tc, test_json_is_null);
  tcase_add_test (tc, test_json_is_not_null);
  tcase_add_test (tc, test_json_int);
  tcase_add_test (tc, test_json_int_neg);
  tcase_add_test (tc, test_json_int_wrong_type);
  tcase_add_test (tc, test_json_int_null_name);
  tcase_add_test (tc, test_json_int_null_placeholder);
  tcase_add_test (tc, test_json_int_corrupted);
  tcase_add_test (tc, test_json_int_missing);
  tcase_add_test (tc, test_json_child_char_success);
  tcase_add_test (tc, test_json_child_char_empty_array);
  tcase_add_test (tc, test_json_child_char_array_null_json);
  tcase_add_test (tc, test_json_child_char_array_null_list_lenght);
  tcase_add_test (tc, test_json_child_char_array_null_parent_name);
  tcase_add_test (tc, test_json_child_char_array_null_array_name);
  tcase_add_test (tc, test_json_child_char_array_null_element_name);
  tcase_add_test (tc, test_json_child_char_array_null_out);
  tcase_add_test (tc, test_json_child_char_array_arrays_parent_wrong_type);
  tcase_add_test (tc, test_json_child_char_array_array_wrong_type);
  tcase_add_test (tc, test_json_child_char_array_arrays_child_wrong_type);
  tcase_add_test (tc, test_json_child_char_array_arrays_child_not_string);
  tcase_add_test (tc, test_json_child_string_sucess);
  tcase_add_test (tc, test_json_child_string_null_json);
  tcase_add_test (tc, test_json_child_string_null_parent);
  tcase_add_test (tc, test_json_child_string_null_name);
  tcase_add_test (tc, test_json_child_string_null_out);
  tcase_add_test (tc, test_json_child_string_invalid_json);
  tcase_add_test (tc, test_json_child_string_missing_parent);
  tcase_add_test (tc, test_json_child_string_missing_object);
  tcase_add_test (tc, test_json_child_string_wrong_type);

  return suite;
}

GST_CHECK_MAIN (libgstc_ping);
