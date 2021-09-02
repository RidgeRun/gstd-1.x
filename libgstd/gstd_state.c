/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include "gstd_state.h"

enum
{
  N_PROPERTIES
};

struct _GstdState
{
  GstdObject parent;

  GstState state;
  GstElement *target;
};

struct _GstdStateClass
{
  GstdObjectClass parent_class;
};

#define GSTD_TYPE_STATE_ENUM (gstd_state_enum_get_type ())
static GType
gstd_state_enum_get_type (void)
{
  static GType state_enum_type = 0;
  static const GEnumValue state_types[] = {
    {GST_STATE_NULL, "NULL", "null"},
    {GST_STATE_READY, "READY", "ready"},
    {GST_STATE_PAUSED, "PAUSED", "paused"},
    {GST_STATE_PLAYING, "PLAYING", "playing"},
    {0, NULL, NULL}
  };

  if (!state_enum_type) {
    state_enum_type = g_enum_register_static ("GstdStateEnum", state_types);
  }
  return state_enum_type;
}

/* Gstd State debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_state_debug);
#define GST_CAT_DEFAULT gstd_state_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdState:
 * A wrapper for the conventional state
 */

G_DEFINE_TYPE (GstdState, gstd_state, GSTD_TYPE_OBJECT);

/* VTable */
static GstdReturnCode
gstd_state_to_string (GstdObject * obj, gchar ** outstring);
static GstdReturnCode
gstd_state_update (GstdObject * object, const gchar * sstate);
static void gstd_state_dispose (GObject * obj);
static GstState gstd_state_read (GstdState * state);

static void
gstd_state_class_init (GstdStateClass * klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstdc = GSTD_OBJECT_CLASS (klass);
  guint debug_color;

  oclass->dispose = gstd_state_dispose;

  gstdc->to_string = GST_DEBUG_FUNCPTR (gstd_state_to_string);
  gstdc->update = GST_DEBUG_FUNCPTR (gstd_state_update);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_state_debug, "gstdstate", debug_color,
      "Gstd State category");

}

static void
gstd_state_init (GstdState * self)
{
  GST_INFO_OBJECT (self, "Initializing state");
  self->state = GST_STATE_NULL;
  self->target = NULL;
}

static GstdReturnCode
gstd_state_to_string (GstdObject * obj, gchar ** outstring)
{
  GstdState *self;
  GValue value = G_VALUE_INIT;
  gchar *svalue;
  const gchar *typename;
  GstdIFormatter *formatter = g_object_new (obj->formatter_factory, NULL);

  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_STATE (obj);

  /* Describe each parameter using a structure */
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "name");
  gstd_iformatter_set_string_value (formatter, GSTD_OBJECT_NAME (self));

  gstd_iformatter_set_member_name (formatter, "value");

  /* The state of the pipeline might have changed autonomously,
     refresh the value */
  self->state = gstd_state_read (self);

  g_value_init (&value, GSTD_TYPE_STATE_ENUM);
  g_value_set_enum (&value, self->state);
  svalue = gst_value_serialize (&value);
  gstd_iformatter_set_string_value (formatter, svalue);

  g_free (svalue);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "param");
  /* Describe the parameter specs using a structure */
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "description");
  gstd_iformatter_set_string_value (formatter, "The state of the pipeline");

  typename = g_type_name (GSTD_TYPE_STATE_ENUM);
  gstd_iformatter_set_member_name (formatter, "type");
  gstd_iformatter_set_string_value (formatter, typename);

  g_value_init (&value, GSTD_TYPE_PARAM_FLAGS);
  g_value_set_flags (&value, G_PARAM_READWRITE);
  svalue = g_strdup_value_contents (&value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (formatter, "access");
  gstd_iformatter_set_string_value (formatter, svalue);

  g_free (svalue);

  /* Close parameter specs structure */
  gstd_iformatter_end_object (formatter);

  /* Close parameter structure */
  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  /* Free formatter */
  g_object_unref (formatter);
  return GSTD_EOK;
}

static GstdReturnCode
gstd_state_update (GstdObject * object, const gchar * sstate)
{
  GstdState *self;
  GstStateChangeReturn gstret;
  GValue value = G_VALUE_INIT;
  GstState state;

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (sstate, GSTD_NULL_ARGUMENT);

  self = GSTD_STATE (object);

  g_value_init (&value, GSTD_TYPE_STATE_ENUM);
  if (!gst_value_deserialize (&value, sstate)) {
    GST_ERROR_OBJECT (self, "Unable to interpret \"%s\" as a state", sstate);
    return GSTD_BAD_VALUE;
  }

  state = g_value_get_enum (&value);
  g_value_unset (&value);

  gstret = gst_element_set_state (self->target, state);
  if (GST_STATE_CHANGE_FAILURE == gstret) {
    GST_ERROR_OBJECT (self, "Failed to change the state of the pipeline");
    return GSTD_STATE_ERROR;
  }

  self->state = state;

  return GSTD_EOK;
}

GstdState *
gstd_state_new (GstElement * target)
{
  GstdState *self;

  g_return_val_if_fail (target, NULL);

  self = g_object_new (GSTD_TYPE_STATE, "name", "state", NULL);
  self->target = gst_object_ref (target);

  return self;
}

static void
gstd_state_dispose (GObject * object)
{
  GstdState *self;

  self = GSTD_STATE (object);

  gst_object_unref (self->target);
  self->target = NULL;

  G_OBJECT_CLASS (gstd_state_parent_class)->dispose (object);
}

static GstState
gstd_state_read (GstdState * self)
{
  GstState current;
  GstState pending;

  g_return_val_if_fail (self, GST_STATE_NULL);

  gst_element_get_state (self->target, &current, &pending, 0);

  return current;
}
