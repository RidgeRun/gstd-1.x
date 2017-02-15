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

#ifndef __GSTD_RETURN_CODES_H__
#define __GSTD_RETURN_CODES_H__

/**
 * \file return_codes.h
 *
 * Gstreamer Daemon return codes
 */

typedef enum _GstdReturnCode GstdReturnCode;

enum _GstdReturnCode
{
  /**
   * Everything went OK 
   */
  GSTD_EOK = 0,

  /**
   * A mandatory argument was passed NULL
   */
  GSTD_NULL_ARGUMENT = 1,

  /**
   * A bad pipeline description was provided
   */
  GSTD_BAD_DESCRIPTION = 2,

  /**
   * The name trying to be used already exist
   */
  GSTD_EXISTING_NAME = 3,

  /**
   * Missing initialization
   */
  GSTD_MISSING_INITIALIZATION = 4,

  /**
   * The requested pipeline was not found
   */
  GSTD_NO_PIPELINE = 5,

  /**
   * The requested resource was not found
   */
  GSTD_NO_RESOURCE = 6,

  /**
   * Cannot create a resource in the given property
   */
  GSTD_NO_CREATE = 7,

  /**
   * The resource to create already exists
   */
  GSTD_EXISTING_RESOURCE = 8,

  /**
   * Cannot update the given property
   */
  GSTD_NO_UPDATE = 9,

  GSTD_BAD_COMMAND = 10,

  GSTD_NO_READ = 11,

  GSTD_NO_CONNECTION = 12,

  GSTD_BAD_VALUE = 13,

  GSTD_STATE_ERROR = 14,

  GSTD_IPC_ERROR = 15
};


#endif //__GSTD_RETURN_CODES_H__
