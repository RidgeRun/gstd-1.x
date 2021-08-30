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

#define HEADER \
      "\nGstD version " PACKAGE_VERSION "\n" \
      "Copyright (C) 2015-2021 RidgeRun (https://www.ridgerun.com)\n\n"

/*
 * GstDManager:
 * Opaque representation of GstD state.
 * This struct will have: Session, GstdIpc and num_ipcs (for now)
 */
typedef struct _GstDManager GstDManager;

/**
 * Supported_IPCs:
 * @GSTD_IPC_TYPE_TCP: To enable TCP communication
 * @GSTD_IPC_TYPE_UNIX: To enable TCP communication
 * @GSTD_IPC_TYPE_HTTP: To enable TCP communication
 * IPC options for libGstD
 */
typedef enum _SupportedIpcs SupportedIpcs; /* Used to avoid importing gstd_ipc.h in this file */

enum _SupportedIpcs 
{
    GSTD_IPC_TYPE_TCP,
    GSTD_IPC_TYPE_UNIX,
    GSTD_IPC_TYPE_HTTP,
};

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
 * gstd_init_get_option_group:
 * 
 * Returns: A GOptionGroup with GStreamer's argument specification.
 * 
 */
GOptionGroup* gstd_init_get_option_group (void);

/**
 * gstd_manager_new:
 * 
 * @supported_ipcs: ipcs the user will use 
 * @num_ipcs: length of supported_ipcs
 * @out: placeholder for newly allocated gstd manager.
 * @gst_group: placeholder for GStreamer's argument specifications
 * @argc: arguments for gst_init
 * @argv: arguments for gst_init
 * 
 * Initializes gstd. If ipc array is not NULL
 * it will initialize the GstdIpc in GstDManager.
 * If it is NULL it will just initialize the session.
 *
 * Returns: GstdStatus indicating success or fail
 */
GstdStatus 
gstd_manager_new (const SupportedIpcs supported_ipcs[], const uint num_ipcs, 
    GstDManager ** out, int argc, char *argv[]);

/**
 * gstd_manager_ipc_options:
 * @manager: The manager returned by gstd_manager_new()
 * @ipc_group: placeholder for IPCs specifications
 * 
 * Get IPCs information into a group 
 *
 */
void
gstd_manager_ipc_options (GstDManager * manager, GOptionGroup * ipc_group[]);

/**
 * gstd_manager_ipc_start:
 * @manager: The manager returned by gstd_manager_new()
 * 
 * Starts the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
int
gstd_manager_ipc_start (GstDManager * manager);

/**
 * gstd_manager_ipc_start:
 * @manager: The manager returned by gstd_manager_new()
 * 
 * Stops the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
void
gstd_manager_ipc_stop (GstDManager * manager);


/**
 * gstd_manager_free:
 * @manager: A valid manager allocated with gstd_new()
 *
 * Frees a previously allocated GstDManager.
 *
 * Returns: A newly allocated GstDManager. Use gstd_free() after
 * usage.
 */
void
gstd_manager_free (GstDManager * manager);


#ifdef __cplusplus
}
#endif

#endif // __LIBGSTD_H__
