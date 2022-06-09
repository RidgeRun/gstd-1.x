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
