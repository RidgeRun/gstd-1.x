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

#include "gstd_action.h"

enum
{
  PROP_TARGET = 1,
  N_PROPERTIES
};

#define DEFAULT_PROP_TARGET NULL

/* Gstd Action debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_action_debug);
#define GST_CAT_DEFAULT gstd_action_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdAction:
 * A wrapper for the conventional action
 */

G_DEFINE_TYPE (GstdAction, gstd_action, GSTD_TYPE_OBJECT);

/* VTable */
static void gstd_action_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_action_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_action_dispose (GObject *);
static GstdReturnCode gstd_action_to_string (GstdObject * obj,
    gchar ** outstring);
GstdReturnCode gstd_action_create_default (GstdObject * object,
    const gchar * name, const gchar * description);

static void
gstd_action_class_init (GstdActionClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass *gstdc = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_action_set_property;
  object_class->get_property = gstd_action_get_property;
  object_class->dispose = gstd_action_dispose;

  properties[PROP_TARGET] =
      g_param_spec_object ("target",
      "Target",
      "The target object owning the action",
      G_TYPE_OBJECT,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);


  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  gstdc->to_string = GST_DEBUG_FUNCPTR (gstd_action_to_string);
  gstdc->create = GST_DEBUG_FUNCPTR (gstd_action_create_default);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_action_debug, "gstdaction", debug_color,
      "Gstd Action category");

}

static void
gstd_action_init (GstdAction * self)
{
  self->target = DEFAULT_PROP_TARGET;
}

static void
gstd_action_dispose (GObject * object)
{
  GstdAction *self = GSTD_ACTION (object);

  if (self->target) {
    g_object_unref (self->target);
    self->target = NULL;
  }

  G_OBJECT_CLASS (gstd_action_parent_class)->dispose (object);
}

static void
gstd_action_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdAction *self = GSTD_ACTION (object);

  switch (property_id) {
    case PROP_TARGET:
      GST_OBJECT_LOCK (self);
      GST_DEBUG_OBJECT (self, "Returning action owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      g_value_set_object (value, self->target);
      GST_OBJECT_UNLOCK (self);
      break;
    default:
      /* We don't have any other action... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_action_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdAction *self = GSTD_ACTION (object);

  switch (property_id) {
    case PROP_TARGET:
      GST_OBJECT_LOCK (self);
      if (self->target)
        g_object_unref (self->target);
      self->target = g_value_dup_object (value);
      GST_DEBUG_OBJECT (self, "Setting action owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      GST_OBJECT_UNLOCK (self);
      break;
    default:
      /* We don't have any other action... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static GstdReturnCode
gstd_action_to_string (GstdObject * obj, gchar ** outstring)
{
  guint action_id;
  GSignalQuery query;
  GstdAction *self;
  const gchar *typename;
  GstdIFormatter *formatter = NULL;
  int i;

  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_ACTION (obj);
  formatter = g_object_new (obj->formatter_factory, NULL);

  action_id =
      g_signal_lookup (GSTD_OBJECT_NAME (self), G_OBJECT_TYPE (self->target));
  g_signal_query (action_id, &query);

  /* Describe each action using a structure */
  gstd_iformatter_begin_object (formatter);

  gstd_iformatter_set_member_name (formatter, "name");
  gstd_iformatter_set_string_value (formatter, query.signal_name);

  gstd_iformatter_set_member_name (formatter, "arguments");
  gstd_iformatter_begin_array (formatter);

  for (i = 0; i < query.n_params; i++) {
    typename = g_type_name (query.param_types[i]);
    gstd_iformatter_set_string_value (formatter, typename);
  }

  gstd_iformatter_end_array (formatter);

  gstd_iformatter_set_member_name (formatter, "return");
  typename = g_type_name (query.return_type);
  gstd_iformatter_set_string_value (formatter, typename);

  /* Close action structure */
  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  return GSTD_EOK;
}


GstdReturnCode
gstd_action_create_default (GstdObject * object, const gchar * name,
    const gchar * description)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdAction *action = NULL;
  GSignalQuery query;
  guint action_id;
  GValue ret_sig = G_VALUE_INIT;
  GValue *args = NULL;
  gchar **arg_list = NULL;

  GST_INFO_OBJECT (action, "Action create");

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  action = GSTD_ACTION (object);

  action_id =
      g_signal_lookup (GSTD_OBJECT_NAME (action),
      G_OBJECT_TYPE (action->target));
  g_signal_query (action_id, &query);

  if (query.n_params > 0 && (!description || g_strcmp0(description, "(null)") == 0 || g_strcmp0(description, "") == 0)) {
    return GSTD_NULL_ARGUMENT;
  } else if (description) {
    arg_list = g_strsplit (description, " ", query.n_params);
  }

  if (g_strv_length (arg_list) != query.n_params) {
    return GSTD_NULL_ARGUMENT;
  }

  /* One additional value to store the instance as first value */
  args = g_new0 (GValue, query.n_params + 1);

  g_value_init (&args[0], G_TYPE_OBJECT);
  g_value_set_object (&args[0], action->target);

  for (guint i = 0; i < query.n_params; i++) {
    g_value_init (&args[i + 1], query.param_types[i]);

    if (query.param_types[i] == G_TYPE_STRING) {
      g_value_set_string (&args[i + 1], arg_list[i]);
    } else if (query.param_types[i] == G_TYPE_INT) {
      g_value_set_int (&args[i + 1], g_ascii_strtoll (arg_list[i], NULL, 10));
    } else if (query.param_types[i] == G_TYPE_UINT) {
      g_value_set_uint (&args[i + 1], (guint) g_ascii_strtoull (arg_list[i], NULL, 10));
    } else if (query.param_types[i] == G_TYPE_UINT64) {
      g_value_set_uint64 (&args[i + 1], (guint64) g_ascii_strtoull (arg_list[i],
              NULL, 10));
    } else if (query.param_types[i] == G_TYPE_BOOLEAN) {
      g_value_set_boolean (&args[i + 1], g_ascii_strcasecmp (arg_list[i], "true") == 0);
    } else if (query.param_types[i] == G_TYPE_FLOAT) {
      g_value_set_float (&args[i + 1], g_ascii_strtod (arg_list[i], NULL));
    } else if (query.param_types[i] == G_TYPE_DOUBLE) {
      g_value_set_double (&args[i + 1], g_ascii_strtod (arg_list[i], NULL));
    } else {
      ret = GSTD_BAD_COMMAND;
      goto out;
    }
  }

  if (query.return_type != G_TYPE_NONE) {
    g_value_init(&ret_sig, query.return_type);
    g_signal_emitv (args, action_id, 0, &ret_sig);

    /* The return value is not required */

    g_value_unset(&ret_sig);
  } else {
    g_signal_emitv (args, action_id, 0, NULL);
  }

out:
  for (guint i = 0; i <= query.n_params; i++)
    g_value_unset (&args[i]);
  g_free (args);
  g_strfreev (arg_list);

  return ret;
}
