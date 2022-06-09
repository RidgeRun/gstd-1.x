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

#include <gst/gst.h>

#include "gstd_signal_reader.h"
#include "gstd_property_reader.h"
#include "gstd_callback.h"
#include "gstd_signal.h"

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_signal_reader_debug);
#define GST_CAT_DEFAULT gstd_signal_reader_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static GstdReturnCode gstd_signal_reader_read (GstdIReader * iface,
    GstdObject * object, const gchar * name, GstdObject ** out);

static GstdReturnCode gstd_signal_reader_read_signal (GstdIReader * iface,
    GstdObject * object, GstdObject ** out);

void gstd_signal_marshal (GClosure * closure, GValue * return_value,
    guint n_param_values, const GValue * param_values,
    gpointer invocation_hint, gpointer marshar_data);

static void gstd_signal_reader_dispose (GObject * object);

typedef struct _GstdSignalReaderClass GstdSignalReaderClass;

struct _GstdSignalReader
{
  GstdPropertyReader parent;

  /* signal handling */
  GstdSignal *target;

  /* wait for signal */
  GMutex signal_lock;
  gboolean waiting_signal;
  GCond signal_call;
  GstdCallback *callback;

};

struct _GstdSignalReaderClass
{
  GstdPropertyReaderClass parent_class;
};

static GstdIReaderInterface *parent_interface = NULL;

static void
gstd_ireader_interface_init (GstdIReaderInterface * iface)
{
  parent_interface = g_type_interface_peek_parent (iface);

  iface->read = gstd_signal_reader_read;
}

G_DEFINE_TYPE_WITH_CODE (GstdSignalReader, gstd_signal_reader,
    GSTD_TYPE_PROPERTY_READER, G_IMPLEMENT_INTERFACE (GSTD_TYPE_IREADER,
        gstd_ireader_interface_init));

static void
gstd_signal_reader_class_init (GstdSignalReaderClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  guint debug_color;

  object_class->dispose = gstd_signal_reader_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_signal_reader_debug, "gstdsignalreader",
      debug_color, "Gstd Signal Reader category");
}

static void
gstd_signal_reader_init (GstdSignalReader * self)
{
  GST_INFO_OBJECT (self, "Initializing signal reader");

  self->target = NULL;

  g_mutex_init (&self->signal_lock);
  g_cond_init (&self->signal_call);
}

static void
gstd_signal_reader_dispose (GObject * object)
{
  GstdSignalReader *self = GSTD_SIGNAL_READER (object);

  g_mutex_clear (&self->signal_lock);
  g_cond_clear (&self->signal_call);
}

static GstdReturnCode
gstd_signal_reader_read (GstdIReader * iface, GstdObject * object,
    const gchar * name, GstdObject ** out)
{
  GstdReturnCode ret;
  GstdObject *resource = NULL;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  /* If the user requested to read a signal, connect to the signal,
   * else, default to the property reading implementation
   */
  if (!g_ascii_strcasecmp ("callback", name)) {
    ret = gstd_signal_reader_read_signal (iface, object, &resource);
  } else if (!g_ascii_strcasecmp ("disconnect", name)) {
    ret = gstd_signal_reader_disconnect (iface);
  } else {
    ret = parent_interface->read (iface, object, name, &resource);
  }

  if (!ret) {
    *out = resource;
  }

  return ret;
}

static GstdReturnCode
gstd_signal_reader_read_signal (GstdIReader * iface,
    GstdObject * object, GstdObject ** out)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdSignalReader *self = GSTD_SIGNAL_READER (iface);
  GObject *target;
  GClosure *closure;
  gulong handler_id;
  guint64 timeout;
  guint64 end_time;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (GSTD_IS_SIGNAL (object), GSTD_BAD_VALUE);
  g_return_val_if_fail (out, GSTD_NULL_ARGUMENT);

  GST_INFO_OBJECT (self, "connecting callback of %s",
      GSTD_OBJECT_NAME (object));

  g_mutex_lock (&self->signal_lock);

  self->waiting_signal = TRUE;
  self->target = GSTD_SIGNAL (object);

  /* get signal owner */
  g_object_get (object, "target", &target, NULL);

  /* connect to signal */
  closure = g_closure_new_simple (sizeof (GClosure), self);
  g_closure_set_marshal (closure, gstd_signal_marshal);
  handler_id =
      g_signal_connect_closure (target, GSTD_OBJECT_NAME (object), closure,
      FALSE);

  GST_DEBUG_OBJECT (object, "waiting signal");

  g_object_get (object, "timeout", &timeout, NULL);
  if (timeout != -1) {
    end_time = g_get_monotonic_time () + timeout;
    while (self->waiting_signal) {
      if (!g_cond_wait_until (&self->signal_call, &self->signal_lock, end_time)) {
        goto out;
      }
    }
  } else {
    while (self->waiting_signal)
      g_cond_wait (&self->signal_call, &self->signal_lock);
  }

  if (self->callback) {
    *out = GSTD_OBJECT (self->callback);
  }

  self->callback = NULL;

out:
  g_object_unref (target);
  g_signal_handler_disconnect (target, handler_id);
  g_mutex_unlock (&self->signal_lock);

  return ret;
}

void
gstd_signal_marshal (GClosure * closure, GValue * return_value,
    guint n_param_values, const GValue * param_values, gpointer invocation_hint,
    gpointer marshar_data)
{
  GstdSignalReader *self = GSTD_SIGNAL_READER (closure->data);

  self->callback =
      gstd_callback_new (GSTD_OBJECT_NAME (self->target), return_value,
      n_param_values, param_values);
  g_mutex_lock (&self->signal_lock);
  self->waiting_signal = FALSE;
  g_cond_signal (&self->signal_call);
  g_mutex_unlock (&self->signal_lock);
}

GstdReturnCode
gstd_signal_reader_disconnect (GstdIReader * iface)
{
  GstdSignalReader *self;

  g_return_val_if_fail (iface, GSTD_NULL_ARGUMENT);

  self = GSTD_SIGNAL_READER (iface);

  g_mutex_lock (&self->signal_lock);
  self->waiting_signal = FALSE;
  g_cond_broadcast (&self->signal_call);
  g_mutex_unlock (&self->signal_lock);

  return GSTD_EOK;
}
