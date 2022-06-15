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

#include "gstd_json_builder.h"
#include "gstd_iformatter.h"
#include <json-glib/json-glib.h>


/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_json_builder_debug);
#define GST_CAT_DEFAULT gstd_json_builder_debug

/* Sets the character to be used when indenting */
#define JSON_INDENT_CHAR   ' '
/* Sets the number of repetitions for each indentation level. */
#define JSON_INDENT_LEVEL  4
/* Sets whether the generated JSON should be pretty printed */
#define JSON_SET_PRETTY    TRUE


typedef struct _GstdJsonBuilderClass GstdJsonBuilderClass;

/**
 * GstdJsonBuilder:
 * A wrapper for the conventional json_builder
 */
struct _GstdJsonBuilder
{
  GObject parent;
  JsonBuilder *json_builder;
};

struct _GstdJsonBuilderClass
{
  GObjectClass parent_class;
};


static void gstd_iformatter_interface_init (GstdIFormatterInterface * iface);

static void gstd_json_builder_finalize (GObject * object);

G_DEFINE_TYPE_WITH_CODE (GstdJsonBuilder, gstd_json_builder, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (GSTD_TYPE_IFORMATTER,
        gstd_iformatter_interface_init));

static void
gstd_json_builder_class_init (GstdJsonBuilderClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  guint debug_color;

  object_class->finalize = gstd_json_builder_finalize;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_json_builder_debug, "gstdjsonbuilder",
      debug_color, "Gstd JSON builder category");
}

static void
gstd_json_builder_init (GstdJsonBuilder * self)
{
  GST_INFO_OBJECT (self, "Initializing Json builder");

  self->json_builder = json_builder_new ();
}

static void
gstd_json_builder_begin_object (GstdIFormatter * iface)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));

  self = GSTD_JSON_BUILDER (iface);
  json_builder_begin_object (self->json_builder);
}

static void
gstd_json_builder_end_object (GstdIFormatter * iface)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));

  self = GSTD_JSON_BUILDER (iface);
  json_builder_end_object (self->json_builder);
}

static void
gstd_json_builder_begin_array (GstdIFormatter * iface)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));

  self = GSTD_JSON_BUILDER (iface);
  json_builder_begin_array (self->json_builder);
}

static void
gstd_json_builder_end_array (GstdIFormatter * iface)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));

  self = GSTD_JSON_BUILDER (iface);
  json_builder_end_array (self->json_builder);
}

static void
gstd_json_set_member_name (GstdIFormatter * iface, const gchar * name)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));

  self = GSTD_JSON_BUILDER (iface);
  json_builder_set_member_name (self->json_builder, name);
}

static void
gstd_json_set_string_value (GstdIFormatter * iface, const gchar * value)
{
  GstdJsonBuilder *self;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));
  g_return_if_fail (value);

  self = GSTD_JSON_BUILDER (iface);
  json_builder_add_string_value (self->json_builder, value);
}

static void
gstd_json_set_value (GstdIFormatter * iface, const GValue * value)
{
  GstdJsonBuilder *self;
  gint64 int64_value;
  guint64 uint64_value;
  gdouble double_value;
  gchar *str_value;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));
  g_return_if_fail (value);

  self = GSTD_JSON_BUILDER (iface);

  switch (G_VALUE_TYPE (value)) {
      /* Since Json format only supports string, boolean, integer and
       * double, only related gtypes are cast to this formats
       */
    case G_TYPE_BOOLEAN:
      json_builder_add_boolean_value (self->json_builder,
          g_value_get_boolean (value));
      break;
    case G_TYPE_INT:
      int64_value = g_value_get_int (value);
      json_builder_add_int_value (self->json_builder, int64_value);
      break;
    case G_TYPE_UINT:
      uint64_value = g_value_get_uint (value);
      json_builder_add_int_value (self->json_builder, uint64_value);
      break;
    case G_TYPE_INT64:
      int64_value = g_value_get_int64 (value);
      json_builder_add_int_value (self->json_builder, int64_value);
      break;
    case G_TYPE_UINT64:
      uint64_value = g_value_get_uint64 (value);
      json_builder_add_int_value (self->json_builder, uint64_value);
      break;
    case G_TYPE_FLOAT:
      double_value = g_value_get_float (value);
      json_builder_add_double_value (self->json_builder, double_value);
      break;
    case G_TYPE_DOUBLE:
      double_value = g_value_get_double (value);
      json_builder_add_double_value (self->json_builder, double_value);
      break;
    case G_TYPE_STRING:
      str_value = g_strdup (g_value_get_string (value));
      json_builder_add_string_value (self->json_builder, str_value);
      g_free (str_value);
      break;
    default:
      /* if the gvalue is not a boolean, integer or float point value, then
       * gvalue is converted to string
       */
      str_value = g_strdup_value_contents (value);
      json_builder_add_string_value (self->json_builder, str_value);
      g_free (str_value);
  }
}

static void
gstd_json_builder_generate (GstdIFormatter * iface, gchar ** outstring)
{
  GstdJsonBuilder *self;
  JsonNode *json_node;
  JsonGenerator *json_generator;
  gchar *json_stream;
  gsize json_stream_length;
  JsonBuilder *json_builder;

  g_return_if_fail (GSTD_IS_JSON_BUILDER (iface));
  self = GSTD_JSON_BUILDER (iface);

  json_builder = self->json_builder;

  json_node = json_builder_get_root (json_builder);

  json_generator = json_generator_new ();
  json_generator_set_root (json_generator, json_node);

  /* Configure json format */
  json_generator_set_indent_char (json_generator, JSON_INDENT_CHAR);
  json_generator_set_indent (json_generator, JSON_INDENT_LEVEL);
  json_generator_set_pretty (json_generator, JSON_SET_PRETTY);

  /* Generates a JSON data stream from generator and returns it as a buffer */
  json_stream = json_generator_to_data (json_generator, &json_stream_length);

  json_node_free (json_node);
  g_object_unref (json_generator);
  /* Resets the state of the builder back to its initial state. */
  json_builder_reset (json_builder);

  *outstring = json_stream;
}

static void
gstd_json_builder_finalize (GObject * object)
{
  GstdJsonBuilder *self = GSTD_JSON_BUILDER (object);
  GST_DEBUG_OBJECT (self, "finalize");

  g_object_unref (self->json_builder);
  G_OBJECT_CLASS (gstd_json_builder_parent_class)->finalize (object);
}

static void
gstd_iformatter_interface_init (GstdIFormatterInterface * iface)
{
  iface->begin_object = gstd_json_builder_begin_object;
  iface->end_object = gstd_json_builder_end_object;
  iface->begin_array = gstd_json_builder_begin_array;
  iface->end_array = gstd_json_builder_end_array;
  iface->set_member_name = gstd_json_set_member_name;
  iface->set_string_value = gstd_json_set_string_value;
  iface->set_value = gstd_json_set_value;
  iface->generate = gstd_json_builder_generate;
}
