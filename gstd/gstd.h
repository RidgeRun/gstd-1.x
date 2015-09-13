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
#ifndef __GSTD_H__
#define __GSTD_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "gstd_return_codes.h"
#include "gstd_debug.h"
#include "gstd_pipeline.h"

/**
 * \brief Initializes Gstreamer Daemon 
 *
 * This function initializes everything that GstD needs to operate properly. 
 * This needs to be called before any GstD utility is called.
 *
 * \param argc A pointer to the argument count passed to the main function.
 * \param argv A pointer to the argument list passed to the main function.
 *
 * \post Resources will be allocated for proper GstD operation.
 */
void
gstd_init (gint *argc, gchar **argv[]);

/**
 * \brief Deinitializes Gstreamer Daemon
 *
 * This function frees everything that was initialized by calling gst_init.
 *
 * \post Resources will be freed and GstD utilities will no longer be
 * available, until gst_init is called again.
 */
void
gstd_deinit ();

GstdReturnCode
gstd_create_pipeline (gchar *name, gchar *description, gchar **outname);

#endif //__GSTD_H__
