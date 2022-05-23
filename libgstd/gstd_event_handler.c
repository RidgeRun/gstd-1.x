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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_event_handler.h"
#include "gstd_event_creator.h"

enum
{
  PROP_RECEIVER = 1,
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdEventHandler
{
  GstdObject parent;

  GObject *receiver;
};

struct _GstdEventHandlerClass
{
  GstdObjectClass parent_class;
};

static void
gstd_event_handler_set_property (GObject *,
    guint, const GValue *, GParamSpec *);
static void gstd_event_handler_dispose (GObject *);

G_DEFINE_TYPE (GstdEventHandler, gstd_event_handler, GSTD_TYPE_OBJECT);

/* Gstd EventHandler debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_event_handler_debug);
#define GST_CAT_DEFAULT gstd_event_handler_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void
gstd_event_handler_class_init (GstdEventHandlerClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_event_handler_set_property;
  object_class->dispose = gstd_event_handler_dispose;

  properties[PROP_RECEIVER] =
      g_param_spec_object ("receiver",
      "Receiver",
      "The object that will receive the event_handler",
      G_TYPE_OBJECT,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_event_handler_debug, "gstdevent_handlerhandler",
      debug_color, "Gstd EventHandler  category");
}

static void
gstd_event_handler_init (GstdEventHandler * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd event_handler resource");
  self->receiver = NULL;
}

static void
gstd_event_handler_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdEventHandler *self = GSTD_EVENT_HANDLER (object);

  switch (property_id) {
    case PROP_RECEIVER:
    {
      GstdICreator *creator;

      self->receiver = g_value_get_object (value);
      GST_INFO_OBJECT (self, "Changed receiver to %p", self->receiver);

      creator = GSTD_ICREATOR (g_object_new (GSTD_TYPE_EVENT_CREATOR,
              "receiver", self->receiver, NULL));
      gstd_object_set_creator (GSTD_OBJECT (self), creator);
      break;
    }
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_event_handler_dispose (GObject * object)
{
  GstdEventHandler *self = GSTD_EVENT_HANDLER (object);

  if (self->receiver) {
    g_object_unref (self->receiver);
    self->receiver = NULL;
  }

  G_OBJECT_CLASS (gstd_event_handler_parent_class)->dispose (object);
}

GstdEventHandler *
gstd_event_handler_new (GObject * receiver)
{
  return GSTD_EVENT_HANDLER (g_object_new (GSTD_TYPE_EVENT_HANDLER, "receiver",
          receiver, "name", "event_handler", NULL));
}
