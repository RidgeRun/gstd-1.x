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

void gstd_list_set_creator (GstdList * self, GstdICreator * creator);
void gstd_list_set_deleter (GstdList * self, GstdIDeleter * deleter);

G_END_DECLS
#endif // __GSTD_LIST_H__
