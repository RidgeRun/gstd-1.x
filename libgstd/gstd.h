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

#ifndef __LIBGSTD_H__
#define __LIBGSTD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <gst/gst.h>
#include <glib-unix.h>

#include "gstd_return_codes.h"

/*
 * GstD:
 * Opaque representation of GstD state.
 */
typedef struct _GstD GstD;

/*
 * GstdObject:
 * Opaque representation of GstD objects.
 */
typedef struct _GstdObject GstdObject;

/**
 * gstd_new:
 * 
 * @out: placeholder for newly allocated gstd instance.
 * @argc: arguments for gst_init
 * @argv: arguments for gst_init
 * 
 * Initializes gstd.
 *
 * Returns: GstdReturnCode indicating success or fail
 */
GstdReturnCode 
gstd_new (GstD ** out, int argc, char *argv[]);

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
 * gstd_start:
 * @gstd: The gstd returned by gstd_new()
 * 
 * Starts the ipc in GstdIpc array
 *
 * Returns: GstdReturnCode indicating success or fail
 */
int
gstd_start (GstD * gstd);

/**
 * gstd_stop:
 * @gstd: The gstd instance returned by gstd_new()
 * 
 * Stops the ipc in GstdIpc array
 *
 * Returns: GstdReturnCode indicating success or fail
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
 * gstd_create:
 * 
 * @gstd: A valid gstd instance allocated with gstd_new()
 * @uri: Path to the resource in which the action will be
 * applied in low level CRUD syntax 
 * @name: Name of the resource to create
 * @description: Description of the resource to create.
 * If description is not needed, it must be NULL.
 * 
 * A new Create call of the argument with the description
 *
 * Returns: GstdReturnCode indicating success or fail
 */
GstdReturnCode
gstd_create (GstD *gstd, const gchar *uri, const gchar *name, const gchar *description);

/**
 * gstd_read:
 * 
 * @gstd: A valid gstd instance allocated with gstd_new()
 * @uri: Path to the resource in which the action will be
 * applied in low level CRUD syntax 
 * @resource: Placeholder for the resource required
 * 
 * A new Read call of the argument
 *
 * Returns: GstdReturnCode indicating success or fail
 */
GstdReturnCode
gstd_read (GstD *gstd, const gchar *uri, GstdObject **resource);

/**
 * gstd_update:
 * 
 * @gstd: A valid gstd instance allocated with gstd_new()
 * @uri: Path to the resource in which the action will be
 * applied in low level CRUD syntax 
 * @value: New value to set the resource 
 * 
 * A new Update call of the argument with the description
 *
 * Returns: GstdReturnCode indicating success or fail
 */
GstdReturnCode
gstd_update (GstD *gstd, const gchar *uri, const gchar *value);

/**
 * gstd_delete:
 * 
 * @gstd: A valid gstd instance allocated with gstd_new()
 * @uri: Path to the resource in which the action will be
 * applied in low level CRUD syntax 
 * @name: Name of the resource to delete
 * 
 * A Delete call to a resource given by the URI
 *
 * Returns: GstdReturnCode indicating success or fail
 */
GstdReturnCode
gstd_delete (GstD *gstd, const gchar *uri, const gchar *name);

#ifdef __cplusplus
}
#endif

#endif /* __LIBGSTD_H__ */
