/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GSTD_RETURN_CODES_H__
#define __GSTD_RETURN_CODES_H__

/**
 * \file return_codes.h
 *
 * Gstreamer Daemon return codes
 */

#include <glib.h>

enum _GstdReturnCode
{
  /**
   * Everything went OK 
   */
  GSTD_EOK,

  /**
   * A mandatory argument was passed NULL
   */
  GSTD_NULL_ARGUMENT,

  /**
   * A bad pipeline description was provided
   */
  GSTD_BAD_DESCRIPTION,

  /**
   * The name trying to be used already exists
   */
  GSTD_EXISTING_NAME,

  /**
   * Missing initialization
   */
  GSTD_MISSING_INITIALIZATION,

  /**
   * The requested pipeline was not found
   */
  GSTD_NO_PIPELINE,

  /**
   * The requested resource was not found
   */
  GSTD_NO_RESOURCE,

  /**
   * Cannot create a resource in the given property
   */
  GSTD_NO_CREATE,

  /**
   * The resource to create already exists
   */
  GSTD_EXISTING_RESOURCE,

  /**
   * Cannot update the given property
   */
  GSTD_NO_UPDATE,

  /**
   * Unknown command
   */
  GSTD_BAD_COMMAND,

  /**
   * Cannot read the given resource
   */
  GSTD_NO_READ,

  /**
   * Cannot connect
   */
  GSTD_NO_CONNECTION,

  /**
   * The given value is incorrect
   */
  GSTD_BAD_VALUE,

  /**
   * Failed to change state of a pipeline
   */
  GSTD_STATE_ERROR,

  /**
   * Failed to start IPC
   */
  GSTD_IPC_ERROR,

  /**
   * Unknown event
   */
  GSTD_EVENT_ERROR,

  /**
   * Incomplete arguments in user input
   */
  GSTD_MISSING_ARGUMENT,

  /**
   * Missing name of the pipeline
   */
  GSTD_MISSING_NAME,

};

typedef enum _GstdReturnCode GstdReturnCode;
const gchar *gstd_return_code_to_string (GstdReturnCode code);

#endif //__GSTD_RETURN_CODES_H__
