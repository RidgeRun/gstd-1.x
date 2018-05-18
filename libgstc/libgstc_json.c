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

#include "libgstc_assert.h"
#include "libgstc_json.h"

GstcStatus
gstc_json_get_int (const char *json, const char *name, int *out)
{
  GstcStatus ret;
  json_t *root;
  json_t *data;
  json_error_t error;

  gstc_assert_and_ret_val (json != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (name != NULL, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (out != NULL, GSTC_NULL_ARGUMENT);

  root = json_loads (json, 0, &error);
  if (!root) {
    ret = GSTC_MALFORMED;
    goto out;
  }

  data = json_object_get (root, name);
  if (!data) {
    ret = GSTC_NOT_FOUND;
    goto unref;
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
