/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
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

#include "gstd_json_builder.h"
#include "gstd_iformatter.h"
#include <json-glib/json-glib.h>


/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_json_builder_debug);
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
  JsonBuilder * jsonBuilder;
};

struct _GstdJsonBuilderClass
{
  GObjectClass parent_class;
};  


static void
gstd_iformatter_interface_init (GstdIFormatterInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GstdJsonBuilder, gstd_json_builder, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GSTD_TYPE_IFORMATTER,
                                                gstd_iformatter_interface_init));

static void
gstd_json_builder_class_init (GstdJsonBuilderClass *klass)
{

}

static void
gstd_json_builder_init (GstdJsonBuilder *self)
{
  GST_INFO_OBJECT(self,"Initializing Json builder");
  
  self->jsonBuilder = json_builder_new ();
}

static void
gstd_json_builder_begin_object (GstdIFormatter *iface)
{
  GstdJsonBuilder *self;
    
  self = GSTD_JSON_BUILDER(iface);
  json_builder_begin_object (self->jsonBuilder);
}

static void
gstd_json_builder_end_object (GstdIFormatter *iface)
{
  GstdJsonBuilder *self;

  self = GSTD_JSON_BUILDER(iface);
  json_builder_end_object (self->jsonBuilder);
}

static void
gstd_json_builder_begin_array (GstdIFormatter *iface)
{
  GstdJsonBuilder *self;

  self = GSTD_JSON_BUILDER(iface);
  json_builder_begin_array (self->jsonBuilder);
}

static void gstd_json_builder_end_array (GstdIFormatter *iface)
{
  GstdJsonBuilder *self;

  self = GSTD_JSON_BUILDER(iface);
  json_builder_end_array (self->jsonBuilder);
}

static void
gstd_json_set_member_name (GstdIFormatter *iface, const gchar * name)
{
  GstdJsonBuilder *self;

  self = GSTD_JSON_BUILDER(iface);
  json_builder_set_member_name (self->jsonBuilder, name);
}

static void
gstd_json_set_member_value (GstdIFormatter *iface, const gchar * value)
{
  GstdJsonBuilder *self;

  self = GSTD_JSON_BUILDER(iface);
  json_builder_add_string_value (self->jsonBuilder, value);
}

static void
gstd_json_builder_generator (GstdIFormatter *iface, gchar **outstring)
{
  GstdJsonBuilder *self;
  JsonNode * jsonNode;
  JsonGenerator * jsonGenerator;
  gchar * jsonStream;
  gsize jsonStreamLength;
  JsonBuilder * jsonBuilder;
  
  self = GSTD_JSON_BUILDER(iface);
  jsonBuilder = self->jsonBuilder;

  jsonNode = json_builder_get_root (jsonBuilder);

  jsonGenerator = json_generator_new ();
  json_generator_set_root (jsonGenerator, jsonNode);

  /* Configure json format */
  json_generator_set_indent_char (jsonGenerator,JSON_INDENT_CHAR);
  json_generator_set_indent (jsonGenerator,JSON_INDENT_LEVEL);
  json_generator_set_pretty (jsonGenerator,JSON_SET_PRETTY);

  /* Generates a JSON data stream from generator and returns it as a buffer */
  jsonStream = json_generator_to_data (jsonGenerator,&jsonStreamLength);

  json_node_free (jsonNode);
  g_object_unref (jsonGenerator);
  /* Resets the state of the builder back to its initial state. */
  json_builder_reset (jsonBuilder);

  *outstring = jsonStream;
}

static void
gstd_iformatter_interface_init (GstdIFormatterInterface *iface)
{
  iface->begin_object = gstd_json_builder_begin_object;
  iface->end_object = gstd_json_builder_end_object;
  iface->begin_array = gstd_json_builder_begin_array;
  iface->end_array = gstd_json_builder_end_array;
  iface->set_member_name = gstd_json_set_member_name;
  iface->set_member_value = gstd_json_set_member_value;
  iface->generator = gstd_json_builder_generator;
}
