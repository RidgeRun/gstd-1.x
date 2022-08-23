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

#include <jansson.h>
#include <stdlib.h>
#include <string.h>

#include "libgstc_assert.h"
#include "libgstc_json.h"

static GstcStatus gstc_json_get_value (const char *json, const char *name,
    json_t ** root, json_t ** out);

static GstcStatus
gstc_json_get_value (const char *json, const char *name, json_t ** root,
    json_t ** out)
{
  GstcStatus ret = GSTC_OK;
  json_error_t error;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (root != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);

  *root = json_loads (json, 0, &error);
  if (!*root) {
    ret = GSTC_MALFORMED;
    goto out;
  }

  *out = json_object_get (*root, name);
  if (!*out) {
    ret = GSTC_NOT_FOUND;
    json_decref (*root);
  }

out:
  return ret;
}

GstcStatus
gstc_json_get_int (const char *json, const char *name, int *out)
{
  GstcStatus ret;
  json_t *root;
  json_t *data;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);

  ret = gstc_json_get_value (json, name, &root, &data);
  if (GSTC_OK != ret) {
    goto out;
  }

  if (!json_is_integer (data)) {
    ret = GSTC_TYPE_ERROR;
    goto unref;
  }

  *out = json_integer_value (data);
  ret = GSTC_OK;

unref:
  json_decref (root);

out:
  return ret;
}

GstcStatus
gstc_json_is_null (const char *json, const char *name, int *out)
{
  GstcStatus ret;
  json_t *root;
  json_t *data;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);

  ret = gstc_json_get_value (json, name, &root, &data);
  if (GSTC_OK != ret) {
    goto out;
  }

  *out = json_is_null (data);
  json_decref (root);

out:
  return ret;
}

GstcStatus
gstc_json_get_child_char_array (const char *json, const char *parent_name,
    const char *array_name, const char *element_name, char **out[],
    int *array_lenght)
{
  GstcStatus ret;
  json_t *root;
  json_t *arrays_parent;
  json_t *array_data;
  json_t *data, *name;
  int i, j;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (parent_name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (array_name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (element_name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (array_lenght != NULL, GSTC_NULL_ARGUMENT);

  ret = gstc_json_get_value (json, parent_name, &root, &arrays_parent);
  if (GSTC_OK != ret) {
    goto out;
  }

  if (!json_is_object (arrays_parent)) {
    ret = GSTC_TYPE_ERROR;
    goto unref;
  }
  array_data = json_object_get (arrays_parent, array_name);

  if (!json_is_array (array_data)) {
    ret = GSTC_TYPE_ERROR;
    goto unref;
  }

  *array_lenght = json_array_size (array_data);

  /* Allocate enough memory for all names */
  *out = malloc ((*array_lenght) * sizeof (char *));

  for (i = 0; i < (*array_lenght); i++) {
    const char *string;

    data = json_array_get (array_data, i);
    if (!json_is_object (data)) {
      ret = GSTC_TYPE_ERROR;
      goto clear_mem;
    }

    name = json_object_get (data, element_name);
    if (!json_is_string (name)) {
      ret = GSTC_TYPE_ERROR;
      goto clear_mem;
    }

    string = json_string_value (name);

    /**
      * Jansson library frees memory after parent object is dereferenced,
      * memory copies are necessary in order to preserve data
      */
    (*out)[i] = malloc (strlen (string) + 1);
    memcpy ((*out)[i], string, strlen (string));
    /* Ensure traling null byte is copied */
    (*out)[i][strlen (string)] = '\0';
  }

unref:
  json_decref (root);

out:
  return ret;

clear_mem:
  /* In case of failure all allocated memory is freed */
  for (j = 0; j < i; j++) {
    free ((*out)[j]);
  }
  free (*out);
  return ret;

}

GstcStatus
gstc_json_child_string (const char *json, const char *parent_name,
    const char *data_name, char **out)
{
  GstcStatus ret;
  json_t *root;
  json_t *parent;
  json_t *data;
  const char *tmp_string;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (parent_name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (data_name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);

  ret = gstc_json_get_value (json, parent_name, &root, &parent);
  if (GSTC_OK != ret) {
    goto out;
  }

  data = json_object_get (parent, data_name);
  if (data == NULL) {
    ret = GSTC_NOT_FOUND;
    goto unref;
  }

  if (!json_is_string (data)) {
    ret = GSTC_TYPE_ERROR;
    goto unref;
  }

  tmp_string = json_string_value (data);
  /* Allocate memory for output */
  *out = malloc ((strlen (tmp_string) + 1) * sizeof (char));
  memcpy (*out, tmp_string, strlen (tmp_string));
  /* Ensure traling null byte is copied */
  (*out)[strlen (tmp_string)] = '\0';
  ret = GSTC_OK;

unref:
  json_decref (root);
out:
  return ret;
}
