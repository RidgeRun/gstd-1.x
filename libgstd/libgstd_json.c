/*
 * GStreamer Daemon - gst-launch on steroids
 * C client library abstracting gstd interprocess communication
 *
 * Copyright (c) 2015-2018 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <jansson.h>
#include <stdlib.h>
#include <string.h>

#include "libgstd_assert.h"
#include "libgstd_json.h"

static GstdStatus gstd_json_get_value (const char *json, const char *name,
    json_t ** root, json_t ** out);

static GstdStatus
gstd_json_get_value (const char *json, const char *name, json_t ** root,
    json_t ** out)
{
  GstdStatus ret = GSTD_LIB_OK;
  json_error_t error;

  gstd_assert_and_ret_val (json != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (root != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (out != NULL, GSTD_LIB_NULL_ARGUMENT);

  *root = json_loads (json, 0, &error);
  if (!*root) {
    ret = GSTD_LIB_OOM;
    goto out;
  }

  *out = json_object_get (*root, name);
  if (!*out) {
    ret = GSTD_LIB_NOT_FOUND;
    json_decref (*root);
  }

out:
  return ret;
}

GstdStatus
gstd_json_get_int (const char *json, const char *name, int *out)
{
  GstdStatus ret;
  json_t *root;
  json_t *data;

  gstd_assert_and_ret_val (json != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (out != NULL, GSTD_LIB_NULL_ARGUMENT);

  ret = gstd_json_get_value (json, name, &root, &data);
  if (GSTD_LIB_OK != ret) {
    goto out;
  }

  if (!json_is_integer (data)) {
    ret = GSTD_LIB_TYPE_ERROR;
    goto unref;
  }

  *out = json_integer_value (data);
  ret = GSTD_LIB_OK;

unref:
  json_decref (root);

out:
  return ret;
}

GstdStatus
gstd_json_is_null (const char *json, const char *name, int *out)
{
  GstdStatus ret;
  json_t *root;
  json_t *data;

  gstd_assert_and_ret_val (json != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (out != NULL, GSTD_LIB_NULL_ARGUMENT);

  ret = gstd_json_get_value (json, name, &root, &data);
  if (GSTD_LIB_OK != ret) {
    goto out;
  }

  *out = json_is_null (data);
  json_decref (root);

out:
  return ret;
}

GstdStatus
gstd_json_get_child_char_array (const char *json,
    const char *array_name, const char *element_name, char **out[],
    int *array_lenght)
{
  GstdStatus ret;
  json_t *root;
  json_t *array_data;
  json_t *data, *name;
  json_error_t error;
  int i, j;

  gstd_assert_and_ret_val (json != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (array_name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (element_name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (out != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (array_lenght != NULL, GSTD_LIB_NULL_ARGUMENT);

  root = json_loads (json, 0, &error);

  array_data = json_object_get (root, array_name);

  if (!json_is_array (array_data)) {
    ret = GSTD_LIB_TYPE_ERROR;
    goto unref;
  }

  *array_lenght = json_array_size (array_data);

  /* Allocate enough memory for all names */
  *out = malloc ((*array_lenght) * sizeof (char *));

  for (i = 0; i < (*array_lenght); i++) {
    const char *string;

    data = json_array_get (array_data, i);
    if (!json_is_object (data)) {
      ret = GSTD_LIB_TYPE_ERROR;
      goto clear_mem;
    }

    name = json_object_get (data, element_name);
    if (!json_is_string (name)) {
      ret = GSTD_LIB_TYPE_ERROR;
      goto clear_mem;
    }

    string = json_string_value (name);

    /**
      * Jansson library frees memory after parent object is dereferenced,
      * memory copies are necessary in order to preserve data
      */
    (*out)[i] = malloc (strlen (string) + 1);
    strncpy ((*out)[i], string, strlen (string));
    /* Ensure traling null byte is copied */
    (*out)[i][strlen (string)] = '\0';
  }

unref:
  json_decref (root);

  return ret;

clear_mem:
  /* In case of failure all allocated memory is freed */
  for (j = 0; j < i; j++) {
    free ((*out)[j]);
  }
  free (*out);
  return ret;

}

GstdStatus
gstd_json_child_string (const char *json, const char *data_name, char **out)
{
  GstdStatus ret;
  json_t *root;
  json_t *data;
  json_error_t error;
  const char *tmp_string;

  gstd_assert_and_ret_val (json != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (data_name != NULL, GSTD_LIB_NULL_ARGUMENT);
  gstd_assert_and_ret_val (out != NULL, GSTD_LIB_NULL_ARGUMENT);

  root = json_loads (json, 0, &error);

  data = json_object_get (root, data_name);
  if (data == NULL) {
    ret = GSTD_LIB_NOT_FOUND;
    goto unref;
  }

  if (!json_is_string (data)) {
    ret = GSTD_LIB_TYPE_ERROR;
    goto unref;
  }

  tmp_string = json_string_value (data);
  /* Allocate memory for output */
  *out = malloc ((strlen (tmp_string) + 1) * sizeof (char));
  strncpy (*out, tmp_string, strlen (tmp_string));
  /* Ensure traling null byte is copied */
  (*out)[strlen (tmp_string)] = '\0';
  ret = GSTD_LIB_OK;

unref:
  json_decref (root);
  return ret;
}
