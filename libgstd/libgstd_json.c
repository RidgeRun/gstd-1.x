/*
 * GStreamer Daemon - gst-launch on steroids
 * C library abstracting gstd
 *
 * Copyright (c) 2015-2021 RidgeRun, LLC (http://www.ridgerun.com)
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

#include "libgstd_json.h"

#include <jansson.h>
#include <stdlib.h>
#include <string.h>

static GstdReturnCode
gstd_json_get_value (const char *json, const char *name, json_t ** root,
    json_t ** out)
{
  GstdReturnCode ret = GSTD_EOK;
  json_error_t error = { 0 };

  g_return_val_if_fail (json != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (root != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out != NULL, GSTD_NULL_ARGUMENT);

  *root = json_loads (json, 0, &error);
  if (!*root) {
    ret = GSTD_OOM;
    goto out;
  }

  *out = json_object_get (*root, name);
  if (!*out) {
    ret = GSTD_NO_RESOURCE;
    json_decref (*root);
  }

out:
  return ret;
}

GstdReturnCode
gstd_json_is_null (const char *json, const char *name, int *out)
{
  GstdReturnCode ret = GSTD_EOK;
  json_t *root = NULL;
  json_t *data = NULL;

  g_return_val_if_fail (json != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out != NULL, GSTD_NULL_ARGUMENT);

  ret = gstd_json_get_value (json, name, &root, &data);
  if (GSTD_EOK != ret) {
    goto out;
  }

  *out = json_is_null (data);
  json_decref (root);

out:
  return ret;
}

GstdReturnCode
gstd_json_get_child_char_array (const char *json,
    const char *array_name, const char *element_name, char **out[],
    int *array_lenght)
{
  GstdReturnCode ret = GSTD_EOK;
  json_t *root = NULL;
  json_t *array_data = NULL;
  json_t *data, *name = NULL;
  json_error_t error = { 0 };
  int out_idx = 0, out_to_clean_idx = 0;

  g_return_val_if_fail (json != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (array_name != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (element_name != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (array_lenght != NULL, GSTD_NULL_ARGUMENT);

  root = json_loads (json, 0, &error);
  if (NULL == root) {
    ret = GSTD_BAD_VALUE;
    goto unref;
  }

  array_data = json_object_get (root, array_name);

  if (!json_is_array (array_data)) {
    ret = GSTD_BAD_VALUE;
    goto unref;
  }

  *array_lenght = json_array_size (array_data);

  /* Allocate enough memory for all names */
  *out = malloc ((*array_lenght) * sizeof (char *));
  if (NULL == out) {
    ret = GSTD_OOM;
    goto unref;
  }

  for (out_idx = 0; out_idx < (*array_lenght); out_idx++) {
    const char *string;

    data = json_array_get (array_data, out_idx);
    if (!json_is_object (data)) {
      ret = GSTD_BAD_VALUE;
      goto clear_mem;
    }

    name = json_object_get (data, element_name);
    if (!json_is_string (name)) {
      ret = GSTD_BAD_VALUE;
      goto clear_mem;
    }

    string = json_string_value (name);

    /**
      * Jansson library frees memory after parent object is dereferenced,
      * memory copies are necessary in order to preserve data
      */
    (*out)[out_idx] = malloc (strlen (string) + 1);
    if (NULL == out[out_idx]) {
      ret = GSTD_OOM;
      goto unref;
    }
    strncpy ((*out)[out_idx], string, strlen (string));
    /* Ensure traling null byte is copied */
    (*out)[out_idx][strlen (string)] = '\0';
  }

unref:
  json_decref (root);

  return ret;

clear_mem:
  /* In case of failure all allocated memory is freed */
  for (out_to_clean_idx = 0; out_to_clean_idx < out_idx; out_to_clean_idx++) {
    free ((*out)[out_to_clean_idx]);
  }
  free (*out);
  return ret;

}

GstdReturnCode
gstd_json_child_string (const char *json, const char *data_name, char **out)
{
  GstdReturnCode ret = GSTD_EOK;
  json_t *root = NULL;
  json_t *data = NULL;
  json_error_t error = { 0 };
  const char *tmp_string = NULL;

  g_return_val_if_fail (json != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (data_name != NULL, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out != NULL, GSTD_NULL_ARGUMENT);

  root = json_loads (json, 0, &error);
  if (NULL == root) {
    ret = GSTD_BAD_VALUE;
    goto unref;
  }

  data = json_object_get (root, data_name);
  if (NULL == data) {
    ret = GSTD_NO_RESOURCE;
    goto unref;
  }

  if (!json_is_string (data)) {
    ret = GSTD_BAD_VALUE;
    goto unref;
  }

  tmp_string = json_string_value (data);
  /* Allocate memory for output */
  *out = malloc ((strlen (tmp_string) + 1) * sizeof (char));
  if (NULL == out) {
    ret = GSTD_OOM;
    goto unref;
  }
  strncpy (*out, tmp_string, strlen (tmp_string));
  /* Ensure traling null byte is copied */
  (*out)[strlen (tmp_string)] = '\0';
  ret = GSTD_EOK;

unref:
  json_decref (root);
  return ret;
}
