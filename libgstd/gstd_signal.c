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

#include "gstd_signal_reader.h"
#include "gstd_signal.h"

enum
{
  PROP_TARGET = 1,
  PROP_TIMEOUT,
  PROP_CALLBACK,
  PROP_DISCONNECT,
  N_PROPERTIES
};

#define DEFAULT_PROP_TARGET NULL
#define DEFAULT_PROP_TIMEOUT -1
#define DEFAULT_PROP_TIMEOUT_MIN -1
#define DEFAULT_PROP_TIMEOUT_MAX G_MAXINT64

/* Gstd Signal debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_signal_debug);
#define GST_CAT_DEFAULT gstd_signal_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdSignal:
 * A wrapper for the conventional signal
 */

G_DEFINE_TYPE (GstdSignal, gstd_signal, GSTD_TYPE_OBJECT);

/* VTable */
static void gstd_signal_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_signal_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_signal_dispose (GObject *);

static void
gstd_signal_class_init (GstdSignalClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_signal_set_property;
  object_class->get_property = gstd_signal_get_property;
  object_class->dispose = gstd_signal_dispose;

  properties[PROP_TARGET] =
      g_param_spec_object ("target",
      "Target",
      "The target object owning the signal",
      G_TYPE_OBJECT,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS | GSTD_PARAM_READ);

  properties[PROP_TIMEOUT] =
      g_param_spec_int64 ("timeout", "Timeout",
      "The quantity of time that messages should be waited for, -1: infinity, "
      "0: no time, n: micro seconds to wait",
      DEFAULT_PROP_TIMEOUT_MIN, DEFAULT_PROP_TIMEOUT_MAX, DEFAULT_PROP_TIMEOUT,
      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_CALLBACK] =
      g_param_spec_object ("callback", "Callback",
      "The signal callback", GSTD_TYPE_OBJECT,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_DISCONNECT] =
      g_param_spec_boolean ("disconnect", "Disconnect",
      "Stop waiting for signal", FALSE,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_signal_debug, "gstdsignal", debug_color,
      "Gstd Signal category");

}

static void
gstd_signal_init (GstdSignal * self)
{
  self->target = DEFAULT_PROP_TARGET;
  self->timeout = DEFAULT_PROP_TIMEOUT;

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_SIGNAL_READER, NULL));
}

static void
gstd_signal_dispose (GObject * object)
{
  GstdSignal *self = GSTD_SIGNAL (object);

  GST_INFO_OBJECT (self, "Disposing %s signal", GSTD_OBJECT_NAME (self));

  if (self->target) {
    g_object_unref (self->target);
    self->target = NULL;
  }

  G_OBJECT_CLASS (gstd_signal_parent_class)->dispose (object);
}



static void
gstd_signal_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdSignal *self = GSTD_SIGNAL (object);

  switch (property_id) {
    case PROP_TARGET:
      GST_DEBUG_OBJECT (self, "Returning signal owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      g_value_set_object (value, self->target);
      break;
    case PROP_TIMEOUT:
      GST_DEBUG_OBJECT (self, "Returning signal timeout %" GST_TIME_FORMAT,
          GST_TIME_ARGS (self->timeout));
      g_value_set_int64 (value, self->timeout);
      break;
    case PROP_CALLBACK:
      GST_DEBUG_OBJECT (self, "Connecting callback");
      break;
    default:
      /* We don't have any other signal... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_signal_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdSignal *self = GSTD_SIGNAL (object);

  switch (property_id) {
    case PROP_TARGET:
      if (self->target)
        g_object_unref (self->target);
      self->target = g_value_dup_object (value);
      GST_DEBUG_OBJECT (self, "Setting signal owner %p (%s)", self->target,
          GST_OBJECT_NAME (self->target));
      break;
    case PROP_TIMEOUT:
      self->timeout = g_value_get_int64 (value);
      GST_DEBUG_OBJECT (self, "Timeout changed to %" GST_TIME_FORMAT,
          GST_TIME_ARGS (self->timeout));
      break;
    default:
      /* We don't have any other signal... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gstd_signal_disconnect (GstdSignal * self)
{
  GstdIReader *reader = GSTD_OBJECT (self)->reader;

  gstd_signal_reader_disconnect (reader);
}
