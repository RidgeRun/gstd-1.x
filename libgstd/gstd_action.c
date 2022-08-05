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
  gint ret_sig = 0;

  GST_INFO_OBJECT (action, "Action create");

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  action = GSTD_ACTION (object);

  action_id =
      g_signal_lookup (GSTD_OBJECT_NAME (action),
      G_OBJECT_TYPE (action->target));
  g_signal_query (action_id, &query);

  if (query.n_params > 0) {
    GST_ERROR_OBJECT (action, "Only actions with no parameters are supported");
    ret = GSTD_BAD_VALUE;
    goto out;
  }

  GST_INFO_OBJECT (action, "Emit to %s", GST_OBJECT_NAME (action->target));
  g_signal_emit_by_name (action->target, name, &ret_sig);

  if (ret_sig != 0) {
    ret = GSTD_BAD_VALUE;
  }

out:
  return ret;
}
