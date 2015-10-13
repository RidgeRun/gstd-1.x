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
 * SECTION:gstd_session
 * @short_description: GstdSession Object
 * @title: GstdSession Object
 * @see_also:#GstdObject
 * @include: gstd/gstd.h
 *
 * # Introduction #
 *
 * A #GstdSession encapsulates a GStreamer Daemon session. It holds the
 * structure of pipelines, elements and properties, and provides
 * mechanisms to the user to interact with them. An application may
 * instanciate several #GstdSession objects, and each one will hold a
 * separate list of pipelines. Unless the specific pipelines share
 * physical resources among them, they should operate independently.
 *
 * A #GstdSession is created and deleted as any other GObject:
 * |[<!-- language="C" -->
 * #include <gstd/gstd.h>
 * 
 * gchar *name;
 * GstdSession *gstd;
 *
 * gstd = g_object_new (GSTD_TYPE_SESSION, "name", "MySession", "port", 3000, NULL);
 * g_object_get (G_OBJECT(gstd), "name", &name, NULL);
 * g_print ("The session name is \"%s\"", name);
 *
 * g_free (name);
 * g_object_unref (gstd);
 * ]|
 *
 * If the g_object_new() notation yet seems too cryptic, the
 * convenience gstd_session_new() wrapper may be used in a similar
 * way.
 *
 * |[<!-- language="C" -->
 * GstdSession *gstd = gstd_session_new ("MySession", 3000);
 * ]|
 *
 * # Design #
 *
 * #GstdSession is resource oriented. This means that it exposes its
 * different resources (pipelines, states, elements, properties,
 * etc...) via unique URIs. These resources can be accessed via four
 * generic
 * [CRUD](https://en.wikipedia.org/wiki/Create,_read,_update_and_delete)
 * operations (create, read, update and delete) that perform different
 * actions over them, accordingly. These altogether form a minimalist
 * ReST server that may easily adapt to different IPCs.
 *
 * Currently, the structure of a #GstdSession is similar to the following:
 *
 * |[
 *  Session
 *  ├── name
 *  ├── port
 *  ╰── pipelines
 *      ├── count
 *      ├── Pipeline1
 *      │   ├── name
 *      │   ├── state
 *      │   ╰── elements
 *      │       ├── count
 *      │       ├── Element1
 *      │       │   ├── name
 *      │       │   ├── Property1
 *      │       │   ├── Property2
 *      │       │   ├── ...
 *      │       │   ╰── PropertyN
 *      │       ├── Element2
 *      │       ├── ...
 *      │       ╰── ElementN
 *      ├── Pipeline2
 *      ├── ...
 *      ╰── PipelineN
 * ]|
 *
 * - So, the state of Pipeline1 can be accessed via
 * |[
 * /pipelines/Pipeline1/state
 * ]|
 * - The amount of elements in Pipeline2 can be accessed via
 * |[
 * /pipelines/Pipeline2/elements/count
 * ]|
 * - Property1 of Element1 in Pipeline3 can be accessed via
 * |[
 * /pipelines/Pipeline2/elements/Element3/Property1
 * ]|
 */

#ifndef __GSTD_SESSION___
#define __GSTD_SESSION___

#include <glib.h>
#include <gstd/gstd_return_codes.h>
#include <gstd/gstd_object.h>

G_BEGIN_DECLS

#define GSTD_TYPE_SESSION \
  (gstd_session_get_type())
#define GSTD_SESSION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_SESSION,GstdSession))
#define GSTD_SESSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_SESSION,GstdSessionClass))
#define GSTD_IS_SESSION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_SESSION))
#define GSTD_IS_SESSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_SESSION))
#define GSTD_SESSION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_SESSION, GstdSessionClass))

typedef struct _GstdSession GstdSession;
typedef struct _GstdSessionClass GstdSessionClass;
GType gstd_session_get_type();

GstdSession *
gstd_new (const gchar *name, const guint16 port);

GstdReturnCode
gstd_pipeline_create (GstdSession *gstd, const gchar *name, const gchar *description);

GstdReturnCode
gstd_pipeline_delete (GstdSession *gstd, const gchar *name);

GstdReturnCode
gstd_element_get (GstdSession *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value);
GstdReturnCode
gstd_element_set (GstdSession *gstd, const gchar *pipe, const gchar *name,
		  const gchar *property, gpointer value);

GstdReturnCode
gstd_pipeline_play (GstdSession *gstd, const gchar *pipe);

GstdReturnCode
gstd_pipeline_null (GstdSession *gstd, const gchar *pipe);

GstdReturnCode
gstd_get_by_uri (GstdSession *gstd, const gchar *uri, GstdObject **node);

G_END_DECLS

#endif //__GSTD_SESSION___
