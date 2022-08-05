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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdarg.h>
#include <gst/gst.h>
#include <json-glib/json-glib.h>

#include "gstd_object.h"
#include "gstd_no_creator.h"
#include "gstd_no_reader.h"
#include "gstd_no_updater.h"
#include "gstd_no_deleter.h"

#include "gstd_json_builder.h"

enum
{
  PROP_NAME = 1,
  N_PROPERTIES                  // NOT A PROPERTY
};

/* Gstd Object debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_object_debug);
#define GST_CAT_DEFAULT gstd_object_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdObject, gstd_object, GST_TYPE_OBJECT);

/* VTable */
static void
gstd_object_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_object_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_object_dispose (GObject *);
static GstdReturnCode
gstd_object_create_default (GstdObject * object, const gchar * name,
    const gchar * description);
static GstdReturnCode
gstd_object_read_default (GstdObject *, const gchar *, GstdObject **);
static GstdReturnCode gstd_object_update_default (GstdObject *, const gchar *);
static GstdReturnCode
gstd_object_delete_default (GstdObject * object, const gchar * name);
static GstdReturnCode
gstd_object_to_string_default (GstdObject * object, gchar ** outstring);
void gstd_object_finalize (GObject * object);

GType
gstd_object_flags_get_type (void)
{
  static GType param_flags_type = 0;
  static const GFlagsValue flags_types[] = {
    {GSTD_PARAM_CREATE, "CREATE", "create"},
    {GSTD_PARAM_READ, "READ", "read"},
    {GSTD_PARAM_UPDATE, "UPDATE", "update"},
    {GSTD_PARAM_DELETE, "DELETE", "delete"},
    {0, NULL, NULL}
  };
  if (!param_flags_type) {
    param_flags_type = g_flags_register_static ("GstdParamFlags", flags_types);
  }
  return param_flags_type;
}

static void
gstd_object_class_init (GstdObjectClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_object_set_property;
  object_class->get_property = gstd_object_get_property;
  object_class->dispose = gstd_object_dispose;
  object_class->finalize = gstd_object_finalize;

  properties[PROP_NAME] =
      g_param_spec_string ("name",
      "Name",
      "The name of the current Gstd session",
      GSTD_OBJECT_DEFAULT_NAME,
      G_PARAM_CONSTRUCT_ONLY |
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | GSTD_PARAM_READ);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  klass->create = gstd_object_create_default;
  klass->read = gstd_object_read_default;
  klass->update = gstd_object_update_default;
  klass->delete = gstd_object_delete_default;
  klass->to_string = gstd_object_to_string_default;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_object_debug, "gstdobject", debug_color,
      "Gstd Object category");
}

static void
gstd_object_init (GstdObject * self)
{
  GST_DEBUG_OBJECT (self, "Initializing gstd object");

  self->name = g_strdup (GSTD_OBJECT_DEFAULT_NAME);
  self->creator = g_object_new (GSTD_TYPE_NO_CREATOR, NULL);
  self->reader = g_object_new (GSTD_TYPE_NO_READER, NULL);
  self->updater = g_object_new (GSTD_TYPE_NO_UPDATER, NULL);
  self->deleter = g_object_new (GSTD_TYPE_NO_DELETER, NULL);
  self->formatter_factory = GSTD_TYPE_JSON_BUILDER;
}

void
gstd_object_finalize (GObject * object)
{
  GstdObject *self = GSTD_OBJECT (object);
  GST_DEBUG_OBJECT (self, "finalize");

  G_OBJECT_CLASS (gstd_object_parent_class)->finalize (object);
}

static void
gstd_object_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdObject *self = GSTD_OBJECT (object);

  switch (property_id) {
    case PROP_NAME:
      GST_DEBUG_OBJECT (self, "Returning object name \"%s\"", self->name);
      g_value_set_string (value, self->name);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      return;
  }
}

static void
gstd_object_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdObject *self = GSTD_OBJECT (object);

  switch (property_id) {
    case PROP_NAME:
      if (self->name)
        g_free (self->name);

      self->name = g_value_dup_string (value);
      GST_INFO_OBJECT (self, "Changed object name to %s", self->name);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      return;
  }
}

static void
gstd_object_dispose (GObject * object)
{
  GstdObject *self = GSTD_OBJECT (object);

  GST_DEBUG_OBJECT (object, "Deinitializing %s object",
      GSTD_OBJECT_NAME (self));

  if (self->name) {
    g_free (self->name);
    self->name = NULL;
  }

  g_clear_object (&self->creator);
  g_clear_object (&self->deleter);
  g_clear_object (&self->updater);
  g_clear_object (&self->reader);

  G_OBJECT_CLASS (gstd_object_parent_class)->dispose (object);
}



static GstdReturnCode
gstd_object_create_default (GstdObject * object, const gchar * name,
    const gchar * description)
{
  GstdObject *out;
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object->creator, GSTD_MISSING_INITIALIZATION);

  ret = gstd_icreator_create (object->creator, name, description, &out);

  if (out)
    g_object_unref (out);

  return ret;
}

static GstdReturnCode
gstd_object_read_default (GstdObject * self, const gchar * name,
    GstdObject ** resource)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (resource, GSTD_NULL_ARGUMENT);

  g_return_val_if_fail (self->reader, GSTD_MISSING_INITIALIZATION);

  return gstd_ireader_read (self->reader, self, name, resource);
}

static GstdReturnCode
gstd_object_update_default (GstdObject * self, const gchar * value)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  g_return_val_if_fail (self->updater, GSTD_MISSING_INITIALIZATION);

  return gstd_iupdater_update (self->updater, self, value);
}


static GstdReturnCode
gstd_object_delete_default (GstdObject * object, const gchar * name)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  g_return_val_if_fail (object->deleter, GSTD_MISSING_INITIALIZATION);

  gstd_ideleter_delete (object->deleter, NULL);

  return GSTD_EOK;
}


static GstdReturnCode
gstd_object_to_string_default (GstdObject * self, gchar ** outstring)
{
  GParamSpec **properties;
  GValue value = G_VALUE_INIT;
  GValue flags = G_VALUE_INIT;
  gchar *sflags;
  guint n, i;
  const gchar *typename;
  GstdIFormatter *formatter = g_object_new (self->formatter_factory, NULL);

  gstd_iformatter_begin_object (formatter);
  gstd_iformatter_set_member_name (formatter, "properties");
  gstd_iformatter_begin_array (formatter);

  properties = g_object_class_list_properties (G_OBJECT_GET_CLASS (self), &n);
  for (i = 0; i < n; i++) {
    /* Describe each parameter using a structure */
    gstd_iformatter_begin_object (formatter);

    gstd_iformatter_set_member_name (formatter, "name");

    gstd_iformatter_set_string_value (formatter, properties[i]->name);

    typename = g_type_name (properties[i]->value_type);

    g_value_init (&value, properties[i]->value_type);
    g_object_get_property (G_OBJECT (self), properties[i]->name, &value);

    gstd_iformatter_set_member_name (formatter, "value");
    gstd_iformatter_set_value (formatter, &value);

    gstd_iformatter_set_member_name (formatter, "param");
    /* Describe the parameter specs using a structure */
    gstd_iformatter_begin_object (formatter);

    g_value_unset (&value);

    g_value_init (&flags, GSTD_TYPE_PARAM_FLAGS);
    g_value_set_flags (&flags, properties[i]->flags);
    sflags = g_strdup_value_contents (&flags);
    g_value_unset (&flags);

    gstd_iformatter_set_member_name (formatter, "description");
    gstd_iformatter_set_string_value (formatter, properties[i]->_blurb);

    gstd_iformatter_set_member_name (formatter, "type");
    gstd_iformatter_set_string_value (formatter, typename);

    gstd_iformatter_set_member_name (formatter, "access");
    gstd_iformatter_set_string_value (formatter, sflags);

    /* Close parameter specs structure */
    gstd_iformatter_end_object (formatter);

    g_free (sflags);
    /* Close parameter structure */
    gstd_iformatter_end_object (formatter);
  }
  g_free (properties);

  gstd_iformatter_end_array (formatter);
  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  /* Free formatter */
  g_object_unref (formatter);
  return GSTD_EOK;
}

GstdReturnCode
gstd_object_create (GstdObject * object, const gchar * name,
    const gchar * description)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);

  return GSTD_OBJECT_GET_CLASS (object)->create (object, name, description);

}

GstdReturnCode
gstd_object_read (GstdObject * object, const gchar * property,
    GstdObject ** resource)
{
  GstdReturnCode ret;

  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);

  ret = GSTD_OBJECT_GET_CLASS (object)->read (object, property, resource);

  return ret;
}

GstdReturnCode
gstd_object_update (GstdObject * object, const gchar * value)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (value, GSTD_NULL_ARGUMENT);

  return GSTD_OBJECT_GET_CLASS (object)->update (object, value);
}

GstdReturnCode
gstd_object_delete (GstdObject * object, const gchar * name)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  return GSTD_OBJECT_GET_CLASS (object)->delete (object, name);
}

GstdReturnCode
gstd_object_to_string (GstdObject * object, gchar ** outstring)
{
  g_return_val_if_fail (GSTD_IS_OBJECT (object), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  return GSTD_OBJECT_GET_CLASS (object)->to_string (object, outstring);
}

void
gstd_object_set_creator (GstdObject * self, GstdICreator * creator)
{
  GstdObject *object;

  g_return_if_fail (self);

  object = GSTD_OBJECT (self);

  if (object->creator != NULL) {
    g_object_unref (object->creator);
  }

  object->creator = creator;
}

void
gstd_object_set_reader (GstdObject * self, GstdIReader * reader)
{
  GstdObject *object;

  g_return_if_fail (self);

  object = GSTD_OBJECT (self);

  if (object->reader != NULL) {
    g_object_unref (object->reader);
  }

  object->reader = reader;
}

void
gstd_object_set_updater (GstdObject * self, GstdIUpdater * updater)
{
  GstdObject *object;

  g_return_if_fail (self);

  object = GSTD_OBJECT (self);

  if (object->updater != NULL) {
    g_object_unref (object->updater);
  }

  object->updater = updater;
}

void
gstd_object_set_deleter (GstdObject * self, GstdIDeleter * deleter)
{
  GstdObject *object;

  g_return_if_fail (self);
  g_return_if_fail (deleter);

  object = GSTD_OBJECT (self);

  if (object->deleter != NULL) {
    g_object_unref (object->deleter);
  }

  object->deleter = deleter;
}
