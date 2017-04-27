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
#include "gstd_pipeline_bus.h"
#include "gstd_msg_reader.h"

enum
{
  PROP_MESSAGE = 1,
  PROP_TIMEOUT,
  N_PROPERTIES                  // NOT A PROPERTY
};


struct _GstdPipelineBus
{
  GstdObject parent;

  GObject *bus;
  gint64 timeout;

};

struct _GstdPipelineBusClass
{
  GstdObjectClass parent_class;
};

static void
gstd_pipeline_bus_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_pipeline_bus_get_property (GObject * object,
  guint property_id, GValue * value, GParamSpec * pspec);
static void gstd_pipeline_bus_dispose (GObject *);

G_DEFINE_TYPE (GstdPipelineBus, gstd_pipeline_bus, GSTD_TYPE_OBJECT);

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_bus_debug);
#define GST_CAT_DEFAULT gstd_pipeline_bus_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO
#define GSTD_PIPELINE_BUS_TIMEOUT_DEFAULT -1
#define GSTD_PIPELINE_BUS_TIMEOUT_MIN -1
#define GSTD_PIPELINE_BUS_TIMEOUT_MAX G_MAXINT64

static void
gstd_pipeline_bus_class_init (GstdPipelineBusClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };

  object_class->set_property = gstd_pipeline_bus_set_property;
  object_class->get_property = gstd_pipeline_bus_get_property;
  object_class->dispose = gstd_pipeline_bus_dispose;

  properties[PROP_MESSAGE] =
    g_param_spec_object ("message",
      "Message",
      "The messages sent to the pipeline",
      GSTD_TYPE_OBJECT,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_TIMEOUT] =
    g_param_spec_int64 ("timeout",
      "Timeout",
      "The quantity of time that messages should be waited for, -1: infinity, 0: immediate, n: nanoseconds to wait",
      GSTD_PIPELINE_BUS_TIMEOUT_MIN,
      GSTD_PIPELINE_BUS_TIMEOUT_MAX,
      GSTD_PIPELINE_BUS_TIMEOUT_DEFAULT,
      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_pipeline_bus_debug, "gstdpipelinebus",
      debug_color, "Gstd Pipeline Bus messages category");
}

static void
gstd_pipeline_bus_init (GstdPipelineBus * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd pipeline bus handler");

  self->timeout = GSTD_PIPELINE_BUS_TIMEOUT_DEFAULT;

  gstd_object_set_reader (GSTD_OBJECT(self),
      g_object_new (GSTD_TYPE_MSG_READER, NULL));
}


GstdPipelineBus *
gstd_pipeline_bus_new (GstBus * bus)
{
  GstdPipelineBus * self;

  g_return_val_if_fail (bus, NULL);

  self = GSTD_PIPELINE_BUS (g_object_new (GSTD_TYPE_PIPELINE_BUS, NULL));
  self->bus = gst_object_ref (bus);

  return self;
}

static void
gstd_pipeline_bus_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);

  switch (property_id) {
  case PROP_TIMEOUT:
      self->timeout = g_value_get_int64 (value);
      GST_INFO_OBJECT (self, "Timeout changed to: %li", self->timeout);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_pipeline_bus_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);

  switch (property_id) {
    case PROP_MESSAGE:
      // FIXME: Serialize the message by overriding to_string(), this is not a real
      // property, is just me being lazy
      g_value_set_object (value, NULL);
      break;
    case PROP_TIMEOUT:
      GST_DEBUG_OBJECT (self, "Returning timeout %" GST_TIME_FORMAT, GST_TIME_ARGS(self->timeout));
      g_value_set_int64 (value, self->timeout);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_pipeline_bus_dispose (GObject * object)
{
  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);

  GST_INFO_OBJECT (self, "Disposing %s pipeline bus", GSTD_OBJECT_NAME (self));

  g_clear_object(&self->bus);

  G_OBJECT_CLASS (gstd_pipeline_bus_parent_class)->dispose (object);
}

GstBus *
gstd_pipeline_bus_get_bus (GstdPipelineBus *self)
{
  g_return_val_if_fail (self, NULL);

  return gst_object_ref (self->bus);
}

