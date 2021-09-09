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
  GSTD_LIB_OOM = -4,
  GSTD_LIB_TYPE_ERROR = -5,
  GSTD_LIB_NOT_FOUND = -7,
  GSTD_LIB_THREAD_ERROR = -11,
  GSTD_LIB_BUS_TIMEOUT = -12,
  GSTD_LIB_LONG_RESPONSE = -14
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


#ifdef __cplusplus
}
#endif

#endif /* __LIBGSTD_H__ */
