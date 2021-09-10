
/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file return_codes.C
 *
 * Gstreamer Daemon return codes textual description
 */

#include "lib_gstd_return_codes.h"

const gchar *
gstd_return_code_to_string (GstdReturnCode code)
{
  static const gchar *code_description[] = {
    [-GSTD_EOK] = "Success",
    [-GSTD_NULL_ARGUMENT] = "Required argument is NULL",
    [-GSTD_BAD_DESCRIPTION] = "Bad pipeline description",
    [-GSTD_EXISTING_NAME] = "Name already exists",
    [-GSTD_MISSING_INITIALIZATION] = "Missing initialization",
    [-GSTD_NO_PIPELINE] = "Pipeline requested doesn't exist",
    [-GSTD_NO_RESOURCE] = "Resource requested doesn't exist",
    [-GSTD_NO_CREATE] = "Cannot create in this resource",
    [-GSTD_EXISTING_RESOURCE] = "Resource already exists",
    [-GSTD_NO_UPDATE] = "Cannot update this resource",
    [-GSTD_BAD_COMMAND] = "Bad command",
    [-GSTD_NO_READ] = "Resource not readable",
    [-GSTD_NO_CONNECTION] = "Cannot connect",
    [-GSTD_BAD_VALUE] = "Bad value",
    [-GSTD_STATE_ERROR] = "State error",
    [-GSTD_IPC_ERROR] = "IPC error",
    [-GSTD_EVENT_ERROR] = "Event error",
    [-GSTD_MISSING_ARGUMENT] = "One or more arguments are missing",
    [-GSTD_MISSING_NAME] = "Name is missing"
  };

  const gint size = sizeof (code_description) / sizeof (gchar *);

  g_return_val_if_fail (0 <= code, "(invalid code)");
  g_return_val_if_fail (size > code, "(invalid code)");

  return code_description[code];
}
