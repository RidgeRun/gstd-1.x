/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __GSTD_OBJECT_H__
#define __GSTD_OBJECT_H__

#include <gstd_return_codes.h>

#include "gstd.h"
#include "gstd_icreator.h"
#include "gstd_ideleter.h"
#include "gstd_iformatter.h"
#include "gstd_ireader.h"
#include "gstd_iupdater.h"

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

typedef struct _GstdObjectClass GstdObjectClass;

struct _GstdObject
{
  GstObject parent;

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

  GType formatter_factory;
};

#define GSTD_OBJECT_NAME(obj) (GSTD_OBJECT(obj)->name)
#define GSTD_OBJECT_CODE(obj) (GSTD_OBJECT(obj)->code)

struct _GstdObjectClass
{
  GstObjectClass parent_class;

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
gstd_object_read (GstdObject * object, const gchar * name,
    GstdObject ** resource);
GstdReturnCode gstd_object_update (GstdObject * object, const gchar * value);
GstdReturnCode gstd_object_delete (GstdObject * object, const gchar * name);
GstdReturnCode gstd_object_to_string (GstdObject * object, gchar ** outstring);

void gstd_object_set_creator (GstdObject * self, GstdICreator * creator);
void gstd_object_set_reader (GstdObject * self, GstdIReader * reader);
void gstd_object_set_updater (GstdObject * self, GstdIUpdater * updater);
void gstd_object_set_deleter (GstdObject * self, GstdIDeleter * deleter);

G_END_DECLS
#endif //__GSTD_OBJECT_H__
