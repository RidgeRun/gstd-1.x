
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

#include "gstd_return_codes.h"

const gchar *
gstd_return_code_to_string (GstdReturnCode code)
{
  static const gchar *code_description[] = {
    "Success",
    "Required argument is NULL",
    "Bad pipeline description",
    "Name already exists",
    "Missing initialization",
    "Pipeline requested doesn't exist",
    "Resource requested doesn't exist",
    "Cannot create in this resource",
    "Resource already exist",
    "Cannot update this resource",
    "Bad command",
    "Resource not readable",
    "Cannot connect",
    "Bad value",
    "State error",
    "IPC error",
    "Unknown type",
    "Event error",
    "One or more arguments are missing",
    "Name is missing"
  };

  const gint size = sizeof (code_description)/sizeof(gchar *);

  g_return_val_if_fail ( 0 <= code, "(invalid code)");
  g_return_val_if_fail (size > code, "(invalid code)");

  return code_description[code];
}
