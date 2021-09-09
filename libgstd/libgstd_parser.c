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

#include "libgstd_parser.h"

#include "gstd_parser.h"

static GstdStatus gstd_return_code_to_gstd_status (const GstdReturnCode code);

static GstdStatus
gstd_return_code_to_gstd_status (const GstdReturnCode code)
{
  GstdStatus code_description[] = {
    [GSTD_EOK] = GSTD_LIB_OK,
    [GSTD_NULL_ARGUMENT] = GSTD_LIB_NULL_ARGUMENT,
    [GSTD_BAD_DESCRIPTION] = GSTD_LIB_BAD_PARAMETER,
    [GSTD_EXISTING_NAME] = GSTD_LIB_EXISTING_RESOURCE,
    [GSTD_MISSING_INITIALIZATION] = GSTD_LIB_BAD_PARAMETER,
    [GSTD_NO_PIPELINE] = GSTD_LIB_NOT_FOUND,
    [GSTD_NO_RESOURCE] = GSTD_LIB_NOT_FOUND,
    [GSTD_NO_CREATE] = GSTD_LIB_BAD_ACTION,
    [GSTD_EXISTING_RESOURCE] = GSTD_LIB_EXISTING_RESOURCE,
    [GSTD_NO_UPDATE] = GSTD_LIB_BAD_ACTION,
    [GSTD_BAD_COMMAND] = GSTD_LIB_BAD_ACTION,
    [GSTD_NO_READ] = GSTD_LIB_BAD_ACTION,
    [GSTD_NO_CONNECTION] = GSTD_LIB_NO_CONNECTION,
    [GSTD_BAD_VALUE] = GSTD_LIB_BAD_PARAMETER,
    [GSTD_STATE_ERROR] = GSTD_LIB_BAD_ACTION,
    [GSTD_IPC_ERROR] = GSTD_LIB_NO_CONNECTION,
    [GSTD_EVENT_ERROR] = GSTD_LIB_BAD_ACTION,
    [GSTD_MISSING_ARGUMENT] = GSTD_LIB_BAD_PARAMETER,
    [GSTD_MISSING_NAME] = GSTD_LIB_BAD_PARAMETER
  };

  const gint size = sizeof (code_description) / sizeof (GstdStatus);

  g_return_val_if_fail (size > code, GSTD_LIB_UNKNOWN_ERROR);

  return code_description[code];
}

GstdStatus
gstd_parser (GstdSession * session, const gchar * cmd, gchar ** response)
{
  GstdStatus ret = GSTD_LIB_OK;
  gchar *output = NULL;

  g_return_val_if_fail (NULL != session, GSTD_LIB_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != cmd, GSTD_LIB_NULL_ARGUMENT);

  ret = gstd_parser_parse_cmd (session, cmd, &output);

  if (response != NULL) {
    *response = g_strdup_printf ("%s", output);
  }

  g_free (output);
  return gstd_return_code_to_gstd_status (ret);
}
