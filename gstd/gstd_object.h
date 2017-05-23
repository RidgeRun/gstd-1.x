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
#ifndef __GSTD_OBJECT_H__
#define __GSTD_OBJECT_H__

#include <glib-object.h>
#include <gstd_return_codes.h>

#include "gstd_icreator.h"
#include "gstd_ireader.h"
#include "gstd_iupdater.h"
#include "gstd_ideleter.h"
#include "gstd_iformatter.h"

typedef struct _GstdIFormatter GstdIFormatter;

G_BEGIN_DECLS
#define GSTD_TYPE_OBJECT \
  (gstd_object_get_type())
#define GSTD_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_OBJECT,GstdObject))
#define GSTD_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_OBJECT,GstdObjectClass))
#define GSTD_IS_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_OBJECT))
#define GSTD_IS_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_OBJECT))
#define GSTD_OBJECT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_OBJECT, GstdObjectClass))
typedef struct _GstdObject GstdObject;
typedef struct _GstdObjectClass GstdObjectClass;

struct _GstdObject
{
  GObject parent;

  /**
   * The name of the object
   */
  gchar *name;

  /**
   * A protection for the object's lock
   */
  GMutex codelock;

  /**
   * The return code set by the last function
   */
  GstdReturnCode code;

  /* CRUD behaviour */
  GstdICreator *creator;
  GstdIReader *reader;
  GstdIUpdater *updater;
  GstdIDeleter *deleter;

  GstdIFormatter * formatter;
};

#define GSTD_OBJECT_NAME(obj) (GSTD_OBJECT(obj)->name)
#define GSTD_OBJECT_CODE(obj) (GSTD_OBJECT(obj)->code)

struct _GstdObjectClass
{
  GObjectClass parent_class;

    GstdReturnCode (*create) (GstdObject * object, const gchar * name,
      const gchar * description);
    GstdReturnCode (*read) (GstdObject * object, const gchar * name,
      GstdObject ** resource);
    GstdReturnCode (*update) (GstdObject * object, const gchar * value);
    GstdReturnCode (*delete) (GstdObject * object, const gchar * name);

    GstdReturnCode (*to_string) (GstdObject * object, gchar ** outstring);
};

GType gstd_object_get_type (void);

#define GSTD_OBJECT_DEFAULT_NAME NULL

#define GSTD_PARAM_CREATE (1 << (G_PARAM_USER_SHIFT+0))
#define GSTD_PARAM_READ   (G_PARAM_READABLE)
#define GSTD_PARAM_UPDATE (1 << (G_PARAM_USER_SHIFT+1))
#define GSTD_PARAM_DELETE (1 << (G_PARAM_USER_SHIFT+2))

#define GSTD_TYPE_PARAM_FLAGS (gstd_object_flags_get_type ())
GType gstd_object_flags_get_type (void);

#define GSTD_PARAM_IS_CREATE(p) (p & GSTD_PARAM_CREATE)
#define GSTD_PARAM_IS_READ(p)   (p & GSTD_PARAM_READ)
#define GSTD_PARAM_IS_UPDATE(p) (p & GSTD_PARAM_UPDATE)
#define GSTD_PARAM_IS_DELETE(p) (p & GSTD_PARAM_DELETE)

GstdReturnCode
gstd_object_create (GstdObject * object, const gchar * name,
    const gchar * description);
GstdReturnCode
gstd_object_read (GstdObject * object, const gchar * name, GstdObject ** resource);
GstdReturnCode
gstd_object_update (GstdObject * object, const gchar *value);
GstdReturnCode gstd_object_delete (GstdObject * object, const gchar * name);
GstdReturnCode gstd_object_to_string (GstdObject * object, gchar ** outstring);

void gstd_object_set_creator (GstdObject * self, GstdICreator * creator);
void gstd_object_set_reader (GstdObject * self, GstdIReader * reader);
void gstd_object_set_updater (GstdObject * self, GstdIUpdater * updater);
void gstd_object_set_deleter (GstdObject * self, GstdIDeleter * deleter);

G_END_DECLS
#endif //__GSTD_OBJECT_H__
