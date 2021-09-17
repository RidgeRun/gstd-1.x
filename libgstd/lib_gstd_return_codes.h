
/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2021 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __GSTD_RETURN_CODES_H__
#define __GSTD_RETURN_CODES_H__

/**
 * \file return_codes.h
 *
 * Gstreamer Daemon return codes
 */

#include <glib.h>

typedef enum _GstdReturnCode GstdReturnCode;
const gchar *gstd_return_code_to_string (GstdReturnCode code);

enum _GstdReturnCode
{
  /**
   * Everything went OK 
   */
  GSTD_EOK = 0,

  /**
   * A mandatory argument was passed NULL
   */
  GSTD_NULL_ARGUMENT = -1,

  /**
   * A bad pipeline description was provided
   */
  GSTD_BAD_DESCRIPTION = -2,

  /**
   * The name trying to be used already exists
   */
  GSTD_EXISTING_NAME = -3,

  /**
   * Missing initialization
   */
  GSTD_MISSING_INITIALIZATION = -4,

  /**
   * The requested pipeline was not found
   */
  GSTD_NO_PIPELINE = -5,

  /**
   * The requested resource was not found
   */
  GSTD_NO_RESOURCE = -6,

  /**
   * Cannot create a resource in the given property
   */
  GSTD_NO_CREATE = -7,

  /**
   * The resource to create already exists
   */
  GSTD_EXISTING_RESOURCE = -8,

  /**
   * Cannot update the given property
   */
  GSTD_NO_UPDATE = -9,

  /**
   * Unknown command
   */
  GSTD_BAD_COMMAND = -10,

  /**
   * Cannot read the given resource
   */
  GSTD_NO_READ = -11,

  /**
   * Cannot connect
   */
  GSTD_NO_CONNECTION = -12,

  /**
   * The given value is incorrect
   */
  GSTD_BAD_VALUE = -13,

  /**
   * Failed to change state of a pipeline
   */
  GSTD_STATE_ERROR = -14,

  /**
   * Failed to start IPC
   */
  GSTD_IPC_ERROR = -15,

  /**
   * Unknown event
   */
  GSTD_EVENT_ERROR = -16,

  /**
   * Incomplete arguments in user input
   */
  GSTD_MISSING_ARGUMENT = -17,

  /**
   * Missing name of the pipeline
   */
  GSTD_MISSING_NAME = -18,

  /**
   * The system has run out of memory
   */
  GSTD_OOM = -19,

  /**
   * Unknown error
   */
  GSTD_UNKNOWN_ERROR = -20,

};


#endif //__GSTD_RETURN_CODES_H__
