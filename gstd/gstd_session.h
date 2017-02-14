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
 * GstdSession *gstd = gstd_session_new ("MySession");
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
 * resource server that may easily adapt to different IPCs.
 *
 * The rationale behind this design is to expose a flexible, loosely
 * coupled, extensible interface for the different IPCs controlling
 * the sessions. In fact, many existing IPCs already follow this
 * standard:
 * - In [D-Bus](https://en.wikipedia.org/wiki/D-Bus) processes offers their
 * services exposing objects represented by paths similar to the ones in 
 * UNIX file systems. Each object offers methods and signals.
 * - A [ReSTful](https://en.wikipedia.org/wiki/Representational_state_transfer)
 * web service exposes its individual resources using URIs and clients use 
 * HTTP verbs (typically GET, POST, UPDATE and DELETE) to interact with them.
 * - [Unix Sockets](https://en.wikipedia.org/wiki/Unix_domain_socket) are 
 * mapped to actual paths in the filesystem, allowing clients and servers
 * to access this hierarchical structure to share data.
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
 *
 * # High Level API #
 *
 * Although it will be typically easier for an IPC to interface with a session 
 * using the low level URI mechanism, a normal human being might not find it
 * intuitive to interact with. For this reason, sitting on top of the URI API,
 * each session exposes a set of high level functions which may seem more natural 
 * and similar to a regular API of other libraries. Each high level function 
 * maps to the URI notation and may be used interchangeably. The following table
 * shows some examples of high level-low level equivalences:
 *
 * <table>
 *   <tr>
 *     <th>High Level API</th>
 *     <th>URI Notation</th> 
 *   </tr>
 *   <tr>
 *     <td>gstd_pipeline_create(name, description)</td>
 *     <td>CREATE /pipelines name description</td>
 *   </tr>
 *   <tr>
 *     <td>gstd_pipeline_get_state(name)</td>
 *     <td>READ /pipelines/name/state</td>
 *   </tr>
 *   <tr>
 *     <td>gstd_pipeline_play(name)</td>
 *     <td>UPDATE /pipelines/name/state GSTD_STATE_PLAY</td>
 *   </tr>
 *   <tr>
 *     <td>gstd_pipeline_delete(name)</td>
 *     <td>DELETE /pipelines/name</td>
 *   </tr>
 *   <tr>
 *     <td>gstd_element_list(pipe)</td>
 *     <td>READ /pipelines/name/elements</td>
 *   </tr>
 *   <tr>
 *     <td>gstd_element_set(pipe, name, property, value, ...)</td>
 *     <td>UPDATE /pipelines/pipe/elements/name property value ....</td>
 *   </tr>
 * </table>
 * This API, however, is more coupled to the server and developing a client
 * using them may potentially break the code if the server's architecture 
 * changes in later releases.
 */

#ifndef __GSTD_SESSION___
#define __GSTD_SESSION___

#include <glib.h>
#include <gstd/gstd_return_codes.h>
#include <gstd/gstd_object.h>
#include <gstd/gstd_pipeline.h>
#include "gstd_list.h"
#include "gstd_debug.h"

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

struct _GstdSession
{
  GstdObject parent;

  /**
   * The list of GstdPipelines created by the user
   */
  GstdList *pipelines;

  /*
   * The current process identifier
   */
  GPid pid;

  /*
   * Object containing debug options
   */
  GstdDebug *debug;
};

struct _GstdSessionClass
{
  GstdObjectClass parent_class;
};

GType gstd_session_get_type (void);


/**
 * gstd_session_new: (constructor)
 * @name: (nullable): The name to assign to the session
 * @port: The port to bind to
 * 
 * Creates a new GStreamer Daemon session with the given @name
 * listening to @port.
 *
 * Returns: (transfer full) (nullable): A new #GstdSession. Free after
 * usage using g_object_unref()
 */
GstdSession *gstd_session_new (const gchar * name);

/**
 * gstd_pipeline_create:
 * @gstd: The #GstdSession where to create the pipeline
 * @name: The name to assign to the new pipeline
 * @description: A gst-launch like description of the pipeline
 * 
 * Creates a new #GstdPipeline in the given @session using
 * @description and assigning @name as the identifier. In the case of
 * an error the pipeline will not be created and the respective error
 * code will be returned.
 *
 * Returns: A #GstdReturnCode describing the result of the operation. 
 * Values other that #GSTD_EOK will not create a pipeline.
 */
GstdReturnCode
gstd_pipeline_create (GstdSession * gstd, const gchar * name,
    const gchar * description);

/**
 * gstd_pipeline_delete:
 * @gstd: The #GstdSession to delete the pipeline from
 * @name: The name of the pipeline to delete
 *
 * Removes the pipeline named after @name from the session @gstd.
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode gstd_pipeline_delete (GstdSession * gstd, const gchar * name);

/**
 * gstd_element_get:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline containing the element
 * @name: The name of the element whose property is to be read
 * @property: The name of the property to read
 * @value: (transfer full) (out): A pointer to a variable to hold
 * the property value.
 *
 * Queries the value of a property of an element, contained 
 * in a pipeline of a given session. If applicable, the returned value
 * should be freed.
 * |[<!-- language="C" -->
 * char *name = NULL;
 * GstdReturnCode ret;
 *
 * ret = gstd_element_get (gstd, "pipe0", "camera", "name", (gpointer)&name);
 * if (GSTD_EOK != ret) {
 *   g_printerr ("Error querying property: %d", ret);
 * } else {
 *   g_print ("The camera name is \"%s\"", name);
 *   g_free (name);
 * }
 * ]|
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode
gstd_element_get (GstdSession * gstd, const gchar * pipe, const gchar * name,
    const gchar * property, gpointer value);

/**
 * gstd_element_set:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline containing the element
 * @name: The name of the element whose property is to be modified
 * @property: The name of the property to modify
 * @value: A variable holding the value to set in the property.
 *
 * Sets a value to a property of an element, contained 
 * in a pipeline of a given session. If applicable, the returned value
 * should be freed.
 * |[<!-- language="C" -->
 * char *name = "camera0";
 * GstdReturnCode ret;
 *
 * ret = gstd_element_get (gstd, "pipe0", "camera", "name", (gpointer)name);
 * if (GSTD_EOK != ret) {
 *   g_printerr ("Error setting property: %d", ret);
 * }
 * ]|
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode
gstd_element_set (GstdSession * gstd, const gchar * pipe, const gchar * name,
    const gchar * property, gpointer value);

/**
 * gstd_pipeline_play:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline to set to playing
 *
 * Sets a pipeline to the playing state.
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode gstd_pipeline_play (GstdSession * gstd, const gchar * pipe);

/**
 * gstd_pipeline_null:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline to set to null
 *
 * Sets a pipeline to the null state.
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode gstd_pipeline_null (GstdSession * gstd, const gchar * pipe);

/**
 * gstd_pipeline_pause:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline to set to paused
 *
 * Sets a pipeline to the paused state.
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode gstd_pipeline_pause (GstdSession * gstd, const gchar * pipe);

/**
 * gstd_pipeline_set_state:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline to set to the given state
 * @state: The #GstdPipelineState to apply to the pipeline
 *
 * Sets a pipeline to the given state.
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode
gstd_pipeline_set_state (GstdSession * gstd, const gchar * pipe,
    const GstdPipelineState state);

/**
 * gstd_pipeline_get_state:
 * @gstd: The #GstdSession to apply the operation to
 * @pipe: The name of the pipeline to query the state
 * @state: (out): The #GstdPipelineState to apply to the pipeline
 *
 * Queries the state of the given pipeline
 *
 * Returns: A #GstdReturnCode describing the result of the operation.
 */
GstdReturnCode
gstd_pipeline_get_state (GstdSession * gstd, const gchar * pipe,
    GstdPipelineState * state);

GstdReturnCode
gstd_get_by_uri (GstdSession * gstd, const gchar * uri, GstdObject ** node);

G_END_DECLS
#endif //__GSTD_SESSION___
