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

/**
 * SECTION:libgstc
 * @short_description: Gst client library for C programming language
 * @title: Gstc Library
 * @see_also:#GstClient
 *
 *
 * The #GstClient library gives user applications access to the
 * GStreamer Daemon using the C programming language.
 *
 * A #GstClient is created along with one or more GStreamer pipelines.
 * The pipelines states are controlled, along with any element
 * properties.  Once an application is done with a pipeline, it can be
 * deleted.  To release all resources assoicated with the #GstClient,
 * the client can be released.
 *
 * Usage Notes:
 *
 * 1 ) None of the gstc_* functions block longer than the wait_time
 *     specified in the gstc_client_new() call.  The libgstc library
 *     is multi-threaded and will handle any transaction with
 *     GStreamer Daemon that takes longer than wait_time to complete.
 *     Call gst_client_release() to free the resources and threads
 *     used by the library.
 *
 * 2 ) The bus wait functions are asynchronous calls where a call back
 *     method is passed. To remove a function from the call back list,
 *     the call back function should return a value of zero.
 *
 * 3 ) Because of the lack of polymoriphism in the C programming
 *     language, values passed into or out of the API are essientally
 *     strings.  Because the calling application know the datatype,
 *     the calling application handles the data conversion.  To make
 *     this simple, when a value is passed into the libgstc API, a
 *     printf style format string is included to make it easy for the
 *     value to be turned into string.  When a value is passed back to
 *     the calling application, a scanf style format string is is
 *     included so the library can turn the string value back into the
 *     datatype most easily handled by the calling application.  The
 *     recommended format strings are listed below for each GStreamer
 *     datatype
 *
 *     GStreamer              printf           sscanf
 *     Datatype               format           format
 *                  REVISIT - fill out the table
 */

#ifndef __LIBGSTC_H__
#define __LIBGSTC_H__

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

#endif // __LIBGSTC_H__
