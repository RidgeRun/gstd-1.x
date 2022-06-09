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

#include "gstd_return_codes.h"

const gchar *
gstd_return_code_to_string (GstdReturnCode code)
{
  static const gchar *code_description[] = {
    [GSTD_EOK] = "Success",
    [GSTD_NULL_ARGUMENT] = "Required argument is NULL",
    [GSTD_BAD_DESCRIPTION] = "Bad pipeline description",
    [GSTD_EXISTING_NAME] = "Name already exists",
    [GSTD_MISSING_INITIALIZATION] = "Missing initialization",
    [GSTD_NO_PIPELINE] = "Pipeline requested doesn't exist",
    [GSTD_NO_RESOURCE] = "Resource requested doesn't exist",
    [GSTD_NO_CREATE] = "Cannot create in this resource",
    [GSTD_EXISTING_RESOURCE] = "Resource already exists",
    [GSTD_NO_UPDATE] = "Cannot update this resource",
    [GSTD_BAD_COMMAND] = "Bad command",
    [GSTD_NO_READ] = "Resource not readable",
    [GSTD_NO_CONNECTION] = "Cannot connect",
    [GSTD_BAD_VALUE] = "Bad value",
    [GSTD_STATE_ERROR] = "State error",
    [GSTD_IPC_ERROR] = "IPC error",
    [GSTD_EVENT_ERROR] = "Event error",
    [GSTD_MISSING_ARGUMENT] = "One or more arguments are missing",
    [GSTD_MISSING_NAME] = "Name is missing",
  };

  const gint size = sizeof (code_description) / sizeof (gchar *);

  g_return_val_if_fail (0 <= code, "(invalid code)");
  g_return_val_if_fail (size > code, "(invalid code)");

  return code_description[code];
}
