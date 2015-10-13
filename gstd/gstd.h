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

/**
 * SECTION:gstd
 * @short_description: GstdCore Object
 * @title: GstdCore Object
 * @see_also:#GstdObject
 * @include: gstd.h
 *
 * A GstdCore encapsulates a GStreamer Daemon session. It holds the
 * structure of pipelines, elements and properties, and provides
 * mechanisms to the user to interact with them. An application may
 * instanciate several GstdCore objects, and each one will hold a
 * separate list of pipelines. Unless the specific pipelines share
 * physical resources among them, they should operate independently.
 *
 * GstdCore is resource oriented. This means that it exposes its
 * different resources (pipelines, states, elements, properties,
 * etc...) via unique URIs, forming a minimalist ReST server.
 *
 * A GstdCore is created and deleted as any other GObject:
 * |[<!-- language="C" -->
 * gchar *name;
 * GstdCore *gstd;
 *
 * gstd = gstd_core_new ("MySession", 3000);
 * g_object_get (G_OBJECT(gstd), "name", &name, NULL);
 * g_print ("The session name is \"%s\"", name);
 *
 * g_free (name);
 * g_object_unref (gstd);
 * ]|
 * 
 */

#ifndef __GSTD_H__
#define __GSTD_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include "gstd_object.h"
#include "gstd_pipeline.h"
#include "gstd_element.h"
#include "gstd_list.h"
#include "gstd_return_codes.h"

G_BEGIN_DECLS

#define GSTD_TYPE_CORE \
  (gstd_core_get_type())
#define GSTD_CORE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_CORE,GstdCore))
#define GSTD_CORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_CORE,GstdCoreClass))
#define GSTD_IS_CORE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_CORE))
#define GSTD_IS_CORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_CORE))
#define GSTD_CORE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_CORE, GstdCoreClass))

typedef struct _GstdCore GstdCore;
typedef struct _GstdCoreClass GstdCoreClass;
GType gstd_core_get_type();

GstdCore *
gstd_new (const gchar *name, const guint16 port);

GstdReturnCode
gstd_pipeline_create (GstdCore *gstd, const gchar *name, const gchar *description);

GstdReturnCode
gstd_pipeline_delete (GstdCore *gstd, const gchar *name);

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
