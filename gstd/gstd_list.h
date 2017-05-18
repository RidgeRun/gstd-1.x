/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
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

#ifndef __GSTD_LIST_H__
#define __GSTD_LIST_H__

#include <glib-object.h>

#include "gstd_object.h"

G_BEGIN_DECLS
/*
 * Type declaration.
 */
#define GSTD_TYPE_LIST \
  (gstd_list_get_type())
#define GSTD_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_LIST,GstdList))
#define GSTD_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_LIST,GstdListClass))
#define GSTD_IS_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_LIST))
#define GSTD_IS_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_LIST))
#define GSTD_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_LIST, GstdListClass))
typedef struct _GstdList GstdList;
typedef struct _GstdListClass GstdListClass;

/**
 * GstdList:
 * A wrapper for the conventional list
 */
struct _GstdList
{
  GstdObject parent;

  guint count;

  GType node_type;

  GParamFlags flags;

  GList *list;
};

struct _GstdListClass
{
  GstdObjectClass parent_class;
};

GType gstd_list_get_type ();

GstdObject * gstd_list_find_child (GstdList *self, const gchar * name);
gboolean gstd_list_append_child (GstdList *, GstdObject *child);

G_END_DECLS
#endif // __GSTD_LIST_H__
