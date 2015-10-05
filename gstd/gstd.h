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
#include "gstd_object.h"
#include "gstd_pipeline.h"
#include "gstd_return_codes.h"

G_BEGIN_DECLS

#define GSTD_TYPE_CORE gstd_core_get_type ()
G_DECLARE_FINAL_TYPE (GstdCore, gstd_core, GSTD, CORE, GstdObject);

GstdCore *
gstd_new (const gchar *name, const guint16 port);

GstdReturnCode
gstd_pipeline_create (GstdCore *gstd, const gchar *name, const gchar *description);

GstdReturnCode
gstd_pipeline_destroy (GstdCore *gstd, const gchar *name);

GstdReturnCode
gstd_element_get (GstdCore *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value);
GstdReturnCode
gstd_element_set (GstdCore *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value);

GstdReturnCode
gstd_pipeline_play (GstdCore *gstd, const gchar *pipe);

GstdReturnCode
gstd_pipeline_null (GstdCore *gstd, const gchar *pipe);

GstdReturnCode
gstd_get_by_uri (GstdCore *gstd, const gchar *uri, GstdObject **node);



G_END_DECLS

#endif //__GSTD_H__
