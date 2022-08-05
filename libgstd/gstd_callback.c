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

#include "gstd_callback.h"

/* Gstd Bus Msg debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_callback_debug);

#define GST_CAT_DEFAULT gstd_callback_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void gstd_callback_dispose (GObject * object);
static GstdReturnCode gstd_callback_to_string (GstdObject * object,
    gchar ** outstring);

G_DEFINE_TYPE (GstdCallback, gstd_callback, GSTD_TYPE_OBJECT);

static void
gstd_callback_class_init (GstdCallbackClass * klass)
{
  GObjectClass *oclass;
  GstdObjectClass *goclass;

  guint debug_color;

  oclass = G_OBJECT_CLASS (klass);
  goclass = GSTD_OBJECT_CLASS (klass);

  oclass->dispose = gstd_callback_dispose;
  goclass->to_string = GST_DEBUG_FUNCPTR (gstd_callback_to_string);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_callback_debug, "gstdcallback", debug_color,
      "Gstd Signal Callback category");

}

static void
gstd_callback_init (GstdCallback * self)
{
  GST_INFO_OBJECT (self, "Initializing callback");
}

static void
gstd_callback_dispose (GObject * object)
{
  GstdCallback *self;
  guint i;

  self = GSTD_CALLBACK (object);

  if (self->signal_name) {
    g_free (self->signal_name);
    self->signal_name = NULL;
  }

  if (self->param_values) {
    for (i = 0; i < self->n_params; i++)
      g_value_unset (self->param_values + i);

    g_free (self->param_values);
    self->param_values = NULL;
    self->n_params = 0;
  }

  G_OBJECT_CLASS (gstd_callback_parent_class)->dispose (object);
}


static GstdReturnCode
gstd_callback_to_string (GstdObject * object, gchar ** outstring)
{
  GstdCallback *self;
  guint i;
  GstdIFormatter *formatter = g_object_new (object->formatter_factory, NULL);

  g_return_val_if_fail (object, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_CALLBACK (object);

  GST_DEBUG_OBJECT (self, "Callback to string %p", object);

  gstd_iformatter_begin_object (formatter);
  gstd_iformatter_set_member_name (formatter, "name");
  gstd_iformatter_set_string_value (formatter, self->signal_name);

  gstd_iformatter_set_member_name (formatter, "arguments");
  gstd_iformatter_begin_array (formatter);

  for (i = 0; i < self->n_params; i++) {
    gstd_iformatter_begin_object (formatter);
    gstd_iformatter_set_member_name (formatter, "type");
    gstd_iformatter_set_string_value (formatter,
        G_VALUE_TYPE_NAME (&self->param_values[i]));

    gstd_iformatter_set_member_name (formatter, "value");
    gstd_iformatter_set_value (formatter, &self->param_values[i]);
    gstd_iformatter_end_object (formatter);
  }

  gstd_iformatter_end_array (formatter);

  gstd_iformatter_end_object (formatter);

  gstd_iformatter_generate (formatter, outstring);

  /* Free formatter */
  g_object_unref (formatter);
  return GSTD_EOK;
}


GstdCallback *
gstd_callback_new (const gchar * signal_name, GValue * return_value,
    guint n_param_values, const GValue * param_values)
{
  GstdCallback *cb;
  GValue *params;
  guint i;

  cb = g_object_new (GSTD_TYPE_CALLBACK, NULL);

  params = g_malloc0 (sizeof (GValue) * (n_param_values));

  for (i = 0; i < n_param_values; i++) {
    GValue *value = params + i;
    g_value_init (value, G_VALUE_TYPE (param_values + i));
    g_value_copy (param_values + i, value);
  }

  cb->param_values = params;
  cb->n_params = n_param_values;
  cb->signal_name = g_strdup (signal_name);

  /* signal return is not handled since the return value
   * will depends of each signal  specific case */

  return cb;
}
