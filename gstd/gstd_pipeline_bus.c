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
#include <unistd.h>
#include "gstd_pipeline_bus.h"


enum
{
  PROP_BUS = 1,
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdPipelineBus
{
  GstdObject parent;

  GObject *bus;

  GQueue *messages;

};

struct _GstdPipelineBusClass
{
  GstdObjectClass parent_class;
};

static void
gstd_pipeline_bus_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void gstd_pipeline_bus_dispose (GObject *);


G_DEFINE_TYPE (GstdPipelineBus, gstd_pipeline_bus, GSTD_TYPE_OBJECT);

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_pipeline_bus_debug);
#define GST_CAT_DEFAULT gstd_pipeline_bus_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void
gstd_pipeline_bus_class_init (GstdPipelineBusClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  object_class->set_property = gstd_pipeline_bus_set_property;
  object_class->dispose = gstd_pipeline_bus_dispose;
  properties[PROP_BUS] =
      g_param_spec_object ("bus",
      "Bus",
      "The bus of the pipeline which wil be used to get events from",
      G_TYPE_OBJECT,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);

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
  self->bus = NULL;
  self->messages = g_queue_new ();
}


GstdPipelineBus *
gstd_pipeline_bus_new (GstBus * bus)
{
  return GSTD_PIPELINE_BUS (g_object_new (GSTD_TYPE_PIPELINE_BUS, "bus", bus,
          NULL));
}

static void
gstd_pipeline_bus_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);

  switch (property_id) {
    case PROP_BUS:
      self->bus = g_value_get_object (value);
      GST_INFO_OBJECT (self, "Changed bus to %p", self->bus);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}


gboolean
gstd_pipeline_bus_read_messages (GstdPipelineBus * self, gchar ** messages)
{
  GstMessage *msg = NULL;
  char *num_messages = NULL;
  char *iter_message = NULL;
  char *iter_tmp_message = NULL;
  GError *error;
  gchar *parsed_txt;
  guint64 currenttime;
  guint64 endtime;

  GST_INFO_OBJECT (self, "Reading pipeline messages ");

  currenttime = g_get_monotonic_time ();
  endtime = g_get_monotonic_time () + 10 * G_TIME_SPAN_SECOND;
  

  while (endtime > currenttime) {
      if(!(msg = gst_bus_timed_pop(GST_BUS(self->bus), 5*GST_SECOND))){
	 GST_INFO_OBJECT (self, "Timeout wating for messages");
      }
      
      if (msg != NULL){
	g_queue_push_tail (self->messages, (gpointer) msg);
      }    
      currenttime = g_get_monotonic_time ();
  }

  num_messages =
      g_strdup_printf ("{\n   \"messages\" : %d\n  }",
      g_queue_get_length (self->messages));

  while (0 < g_queue_get_length (self->messages)) {
    msg = (GstMessage *) g_queue_pop_head (self->messages);
    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:{
        gst_message_parse_error (msg, &error, &parsed_txt);
        iter_tmp_message =
            g_strdup_printf ("{\n GST_MESSAGE_ERROR : %s\n  }", parsed_txt);
        gst_message_unref (msg);
        iter_message = g_strjoin (NULL, iter_message, iter_tmp_message, NULL);
        g_free (iter_tmp_message);
        break;
      }
      default:
        gst_message_unref (msg);
        break;
    }
  }
  *messages =
      g_strdup_printf ("{\n    %s : %s\n  }", num_messages, iter_message);
  g_free (num_messages);

  if (iter_message)
    g_free (iter_message);

  return TRUE;
}


static void
gstd_pipeline_bus_dispose (GObject * object)
{
  GstdPipelineBus *self = GSTD_PIPELINE_BUS (object);

  GST_INFO_OBJECT (self, "Disposing %s pipeline bus", GSTD_OBJECT_NAME (self));

  if (self->messages) {
    g_queue_free (self->messages);
    self->messages = NULL;
  }

  G_OBJECT_CLASS (gstd_pipeline_bus_parent_class)->dispose (object);
}
