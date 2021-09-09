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

#ifndef __LIBGSTD_H__
#define __LIBGSTD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <gst/gst.h>
#include <glib-unix.h>

/*
 * GstD:
 * Opaque representation of GstD state.
 * This struct will have: Session, GstdIpc and num_ipcs (for now)
 */
typedef struct _GstD GstD;

/**
 * GstdStatus:
 * @GSTD_LIB_OK: Everything went okay
 * @GSTD_LIB_NULL_ARGUMENT: A mandatory argument was passed in as NULL
 * @GSTD_LIB_OOM: The system has run out of memory
 * @GSTD_LIB_TYPE_ERROR: An error occurred parsing a type from a string
 * @GSTD_LIB_NOT_FOUND: The response is missing the field requested
 * @GSTD_LIB_THREAD_ERROR: Unable to create a new thread
 * @GSTD_LIB_BUS_TIMEOUT: A timeout was received while waiting on the bus
 * @GSTD_LIB_LONG_RESPONSE: The response exceeds our maximum, typically
 * meaning a missing null terminator
 *
 * Return codes for the different libgstd operations
 */

typedef enum
{
  GSTD_LIB_OK = 0,
  GSTD_LIB_NULL_ARGUMENT = -1,
  GSTD_LIB_BAD_PARAMETER = -2,
  GSTD_LIB_EXISTING_RESOURCE = -3,
  GSTD_LIB_OOM = -4,
  GSTD_LIB_TYPE_ERROR = -5,
  GSTD_LIB_BAD_ACTION = -6,
  GSTD_LIB_NOT_FOUND = -7,
  GSTD_LIB_NO_CONNECTION = -8,
  GSTD_LIB_THREAD_ERROR = -9,
  GSTD_LIB_BUS_TIMEOUT = -10,
  GSTD_LIB_LONG_RESPONSE = -11,
  GSTD_LIB_UNKNOWN_ERROR = -12
} GstdStatus;

/**
 * gstd_context_add_group:
 * 
 * @gstd: The gstd returned by gstd_new()
 * @out: GOptionContext which will contain the GstDOptions
 * 
 */
void
gstd_context_add_group (GstD *gstd, GOptionContext *context);

/**
 * gstd_new:
 * 
 * @out: placeholder for newly allocated gstd instance.
 * @argc: arguments for gst_init
 * @argv: arguments for gst_init
 * 
 * Initializes gstd.
 *
 * Returns: GstdStatus indicating success or fail
 */
GstdStatus 
gstd_new (GstD ** out, int argc, char *argv[]);


/**
 * gstd_start:
 * @gstd: The gstd returned by gstd_new()
 * 
 * Starts the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
int
gstd_start (GstD * gstd);

/**
 * gstd_stop:
 * @gstd: The gstd instance returned by gstd_new()
 * 
 * Stops the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
void
gstd_stop (GstD * gstd);

/**
 * gstd_free:
 * @gstd: A valid gstd instance allocated with gstd_new()
 *
 * Frees a previously allocated GstD.
 *
 * Returns: A newly allocated GstD. Use gstd_free() after
 * usage.
 */
void
gstd_free (GstD * gstd);

/**
 * gstd_pipeline_create:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipeline_name: Name to associate to the pipeline
 * @pipeline_desc: The gst-launch style pipeline description to create
 *
 * Creates a new GStreamer pipeline that can be referred to using
 * @pipeline_name.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_create (GstD * gstd, const char *pipeline_name,
    const char *pipeline_desc);
  
/**
 * gstd_pipeline_list:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipelines: List of existing pipelines names returned by the gstd library
 * @list_lenght: Number of elements in the pipelines list
 *
 * Returns a list of the names of the existing pipelines.  Depending on the
 * deployment, another  application may have created some of the pipelines.
 * The gstd application needs to do a free(*pipelines) and a
 * free(*pipelines[idx]) to release the resources used to hold the list and
 * its elements
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus 
gstd_pipeline_list(GstD * gstd, 
    char **pipelines[], int *list_lenght);

/**
 * gstd_pipeline_delete:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Deletes a previously created GStreamer pipeline named @pipeline_name.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_delete(GstD * gstd, const char *pipeline_name);

/**
 * gstd_pipeline_play:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the play state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_play(GstD * gstd, const char *pipeline_name);

/**
 * gstd_pipeline_pause:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the paused state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_pause(GstD * gstd, const char *pipeline_name);

/**
 * gstd_pipeline_stop:
 * @gstd: The gstd instance returned by gstd_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the null state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_stop(GstD * gstd, const char *pipeline_name);

/**
 * gstd_debug:
 * @threshold: the debug level takes a keyword and the debug level in the argument
 * recieving 0 as a level is equivalent to disabling debug
 * @colors: if non-zero ANSI color control escape sequences will be included in the debug output
 * @reset: if non-zero the debug threshold will be cleared each time, otherwise threshold 
 * is appended to previous threshold.
 *
 * Controls amount of GStreamer Daemon debug logging.  Typically the GStreamer Daemon debug log output is directed to the system log file.
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon timeout
 */
GstdStatus gstd_set_debug (GstD * gstd, const char* threshold,
    const int colors, const int reset);

#ifdef __cplusplus
}
#endif

#endif /* __LIBGSTD_H__ */
