/*
 * This file is part of GStreamer Daemon
 * Based on GStreamer gst-stats application
 * 
 * Copyright 2015-2023 RidgeRun, LLC (http://www.ridgerun.com)
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

#include <gst/gst.h>
#include <json-glib/json-glib.h>

#include "gstd_object.h"
#include "gstd_property_reader.h"
#include "gstd_stats.h"

/* Gstd Stats debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_stats_cat);

enum
{
  PROP_ENABLE = 1,
  PROP_STATS,
  N_PROPERTIES
};

#define PROP_ENABLE_DEFAULT FALSE
#define PROP_STATS_DEFAULT NULL

struct _GstdStats
{
  GstdObject parent;

  /*
   * Enables/Disables stats output.
   */
  gboolean enable;

  /*
   * Current stats
   */
  gchar *stats;

  GRegex *raw_log;
  GRegex *ansi_log;

  /* Stats */
  GHashTable *threads;
  GPtrArray *elements;
  GPtrArray *pads;
  GHashTable *latencies;
  GHashTable *element_latencies;
  GQueue *element_reported_latencies;
  guint64 num_buffers;
  guint64 num_events;
  guint64 num_messages;
  guint64 num_queries;
  guint num_elements;
  guint num_bins;
  guint num_pads;
  guint num_ghostpads;
  GstClockTime last_ts;
  guint total_cpuload;
  gboolean have_cpuload;
  gboolean have_latency;
  gboolean have_element_latency;
  gboolean have_element_reported_latency;

};

struct _GstdStatsClass
{
  GstdObjectClass parent_class;
};

typedef struct
{
  /* display name of the element */
  gchar *name;
  /* the number of latencies counted  */
  guint64 count;
  /* the total of all latencies */
  guint64 total;
  /* the min of all latencies */
  guint64 min;
  /* the max of all latencies */
  guint64 max;
  GstClockTime first_latency_ts;
} GstLatencyStats;

typedef struct
{
  /* The element name */
  gchar *element;
  /* The timestamp of the reported latency */
  guint64 ts;
  /* the min reported latency */
  guint64 min;
  /* the max reported latency */
  guint64 max;
} GstReportedLatency;

typedef struct
{
  /* human readable pad name and details */
  gchar *name, *type_name;
  guint index;
  gboolean is_ghost_pad;
  GstPadDirection dir;
  /* buffer statistics */
  guint num_buffers;
  guint num_live, num_decode_only, num_discont, num_resync, num_corrupted,
      num_marker, num_header, num_gap, num_droppable, num_delta;
  guint min_size, max_size, avg_size;
  /* first and last activity on the pad, expected next_ts */
  GstClockTime first_ts, last_ts, next_ts;
  /* in which thread does it operate */
  gpointer thread_id;
  /* hierarchy */
  guint parent_ix;
} GstPadStats;

typedef struct
{
  /* human readable element name */
  gchar *name, *type_name;
  guint index;
  gboolean is_bin;
  /* buffer statistics */
  guint recv_buffers, sent_buffers;
  guint64 recv_bytes, sent_bytes;
  /* event, message statistics */
  guint num_events, num_messages, num_queries;
  /* first activity on the element */
  GstClockTime first_ts, last_ts;
  /* hierarchy */
  guint parent_ix;
} GstElementStats;

typedef struct
{
  /* time spend in this thread */
  GstClockTime tthread;
  guint cpuload;
} GstThreadStats;

/**
 * GstdStats:
 * A wrapper for the tracers stats
 */

G_DEFINE_TYPE (GstdStats, gstd_stats, GSTD_TYPE_OBJECT);

/* VTable */
static void gstd_stats_set_property (GObject *, guint, const GValue *,
    GParamSpec *);
static void gstd_stats_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_stats_dispose (GObject * obj);
static gchar *gstd_stats_get_json (GstdStats * self);
static void gstd_stats_log_monitor (GstDebugCategory * category,
    GstDebugLevel level, const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer user_data);

static void free_element_stats (gpointer data);
static void free_pad_stats (gpointer data);
static void free_thread_stats (gpointer data);
static void free_latency_stats (gpointer data);
static void free_reported_latency (gpointer data);

static void
gstd_stats_class_init (GstdStatsClass * klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  oclass->dispose = gstd_stats_dispose;
  oclass->get_property = gstd_stats_get_property;
  oclass->set_property = gstd_stats_set_property;

  properties[PROP_ENABLE] =
      g_param_spec_boolean ("enable",
      "Enable",
      "Enable stats collection",
      PROP_ENABLE_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_STATS] =
      g_param_spec_string ("stats",
      "Stats",
      "Current stats collected",
      PROP_STATS_DEFAULT, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (oclass, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_stats_cat, "gstdstats", debug_color,
      "Gstd Stats category");
}

static void
gstd_stats_init (GstdStats * self)
{
  GST_INFO_OBJECT (self, "Initializing stats");

  self->enable = PROP_ENABLE_DEFAULT;
  self->stats = PROP_STATS_DEFAULT;

  gstd_object_set_reader (GSTD_OBJECT (self),
      g_object_new (GSTD_TYPE_PROPERTY_READER, NULL));

  /* log parser */
  self->raw_log = g_regex_new (
      /* 1: ts */
      "^([0-9:.]+) +"
      /* 2: pid */
      "([0-9]+) +"
      /* 3: thread */
      "(0?x?[0-9a-fA-F]+) +"
      /* 4: level */
      "([A-Z]+) +"
      /* 5: category */
      "([a-zA-Z_-]+) +"
      /* 6: file:line:func: */
      "([^:]*:[0-9]+:[^:]*:) +"
      /* 7: (obj)? log-text */
      "(.*)$", 0, 0, NULL);
  self->ansi_log = g_regex_new (
      /* 1: ts */
      "^([0-9:.]+) +"
      /* 2: pid */
      "\\\x1b\\[[0-9;]+m *([0-9]+)\\\x1b\\[00m +"
      /* 3: thread */
      "(0x[0-9a-fA-F]+) +"
      /* 4: level */
      "(?:\\\x1b\\[[0-9;]+m)?([A-Z]+) +\\\x1b\\[00m +"
      /* 5: category */
      "\\\x1b\\[[0-9;]+m +([a-zA-Z_-]+) +"
      /* 6: file:line:func: */
      "([^:]*:[0-9]+:[^:]*:)(?:\\\x1b\\[00m)? +"
      /* 7: (obj)? log-text */
      "(.*)$", 0, 0, NULL);

  /* global statistics */
  self->threads = g_hash_table_new_full (NULL, NULL, NULL, free_thread_stats);
  self->elements = g_ptr_array_new_with_free_func (free_element_stats);
  self->pads = g_ptr_array_new_with_free_func (free_pad_stats);
  self->latencies =
      g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_latency_stats);
  self->element_latencies =
      g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_latency_stats);
  self->element_reported_latencies = g_queue_new ();;
  self->num_buffers = 0;
  self->num_events = 0;
  self->num_messages = 0;
  self->num_queries = 0;
  self->num_elements = 0;
  self->num_bins = 0;
  self->num_pads = 0;
  self->num_ghostpads = 0;
  self->last_ts = G_GUINT64_CONSTANT (0);
  self->total_cpuload = 0;
  self->have_cpuload = FALSE;

  self->have_latency = FALSE;
  self->have_element_latency = FALSE;
  self->have_element_reported_latency = FALSE;

  gst_debug_add_log_function (gstd_stats_log_monitor, self, NULL);
}

static void
gstd_stats_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdStats *self = GSTD_STATS (object);

  switch (property_id) {
    case PROP_ENABLE:
      GST_LOG_OBJECT (self, "Returning stats enabled %d", self->enable);
      g_value_set_boolean (value, self->enable);
      break;
    case PROP_STATS:
      if (self->stats) {
        g_free (self->stats);
      }
      self->stats = gstd_stats_get_json (self);
      GST_DEBUG_OBJECT (self, "Returning current stats %s", self->stats);
      g_value_set_string (value, self->stats);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_stats_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdStats *self = GSTD_STATS (object);

  switch (property_id) {
    case PROP_ENABLE:
      self->enable = g_value_get_boolean (value);
      GST_DEBUG_OBJECT (self, "Changing stats enabled to %d", self->enable);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_stats_dispose (GObject * object)
{
  GstdStats *self;

  self = GSTD_STATS (object);

  GST_ERROR_OBJECT (self, "Deinitializing gstd stats");

  if (self->stats) {
    g_free (self->stats);
    self->stats = NULL;
  }

  if (self->pads)
    g_ptr_array_free (self->pads, TRUE);
  if (self->elements)
    g_ptr_array_free (self->elements, TRUE);
  if (self->threads)
    g_hash_table_destroy (self->threads);

  if (self->latencies) {
    g_hash_table_remove_all (self->latencies);
    g_hash_table_destroy (self->latencies);
    self->latencies = NULL;
  }
  if (self->element_latencies) {
    g_hash_table_remove_all (self->element_latencies);
    g_hash_table_destroy (self->element_latencies);
    self->element_latencies = NULL;
  }
  if (self->element_reported_latencies) {
    g_queue_free_full (self->element_reported_latencies, free_reported_latency);
    self->element_reported_latencies = NULL;
  }

  if (self->raw_log)
    g_regex_unref (self->raw_log);
  if (self->ansi_log)
    g_regex_unref (self->ansi_log);

  G_OBJECT_CLASS (gstd_stats_parent_class)->dispose (object);
}

GstdStats *
gstd_stats_new (void)
{
  GstdStats *self;
  self = GSTD_STATS (g_object_new (GSTD_TYPE_STATS, "name", "stats", NULL));

  GST_ERROR_OBJECT (self, "New stats object");

  return self;
}

/* JSON methods */
static void
get_pads_json (gpointer data, gpointer user_data)
{
  GstPadStats *stats = NULL;
  JsonObject *root = NULL;
  JsonObject *new = NULL;
  gchar *first_ts = NULL;
  gchar *thread_id = NULL;

  g_return_if_fail ((GstPadStats *) data);
  g_return_if_fail ((JsonObject *) user_data);

  stats = (GstPadStats *) data;
  root = (JsonObject *) user_data;
  new = json_object_new ();

  /* Create new pad object */
  first_ts =
      g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (stats->first_ts));
  thread_id = g_strdup_printf ("0x%lx", (guint64) stats->thread_id);
  json_object_set_string_member (new, "type_name", stats->type_name);
  json_object_set_string_member (new, "thread_id", thread_id);
  json_object_set_int_member (new, "index", stats->index);
  json_object_set_boolean_member (new, "is_ghost_pad", stats->is_ghost_pad);
  json_object_set_string_member (new, "direction",
      stats->dir == 0 ? "unknown" : (stats->dir == 1 ? "src" : "sink"));
  json_object_set_int_member (new, "num_buffers", stats->num_buffers);
  json_object_set_int_member (new, "num_live", stats->num_live);
  json_object_set_int_member (new, "num_decode_only", stats->num_decode_only);
  json_object_set_int_member (new, "num_discont", stats->num_discont);
  json_object_set_int_member (new, "num_resync", stats->num_resync);
  json_object_set_int_member (new, "num_corrupted", stats->num_corrupted);
  json_object_set_int_member (new, "num_marker", stats->num_marker);
  json_object_set_int_member (new, "num_header", stats->num_header);
  json_object_set_int_member (new, "num_gap", stats->num_gap);
  json_object_set_int_member (new, "num_droppable", stats->num_droppable);
  json_object_set_int_member (new, "num_delta", stats->num_delta);
  json_object_set_int_member (new, "min_size", stats->min_size);
  json_object_set_int_member (new, "max_size", stats->max_size);
  json_object_set_int_member (new, "avg_size", stats->avg_size);
  json_object_set_string_member (new, "first_ts", first_ts);

  /* Add pad object */
  json_object_set_object_member (root, stats->name, new);

  g_free (first_ts);
  g_free (thread_id);
}

static void
get_elements_json (gpointer data, gpointer user_data)
{
  GstElementStats *stats = NULL;
  JsonObject *root = NULL;
  JsonObject *new = NULL;
  gchar *first_ts = NULL;

  g_return_if_fail ((GstElementStats *) data);
  g_return_if_fail ((JsonObject *) user_data);

  stats = (GstElementStats *) data;
  root = (JsonObject *) user_data;
  new = json_object_new ();

  /* Create new element object */
  first_ts =
      g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (stats->first_ts));
  json_object_set_string_member (new, "type_name", stats->type_name);
  json_object_set_int_member (new, "index", stats->index);
  json_object_set_boolean_member (new, "is_bin", stats->is_bin);
  json_object_set_int_member (new, "recv_buffers", stats->recv_buffers);
  json_object_set_int_member (new, "sent_buffers", stats->sent_buffers);
  json_object_set_int_member (new, "recv_bytes", stats->recv_bytes);
  json_object_set_int_member (new, "sent_bytes", stats->sent_bytes);
  json_object_set_int_member (new, "num_events", stats->num_events);
  json_object_set_int_member (new, "num_messages", stats->num_messages);
  json_object_set_int_member (new, "num_queries", stats->num_queries);
  json_object_set_string_member (new, "first_ts", first_ts);

  /* Add element object */
  json_object_set_object_member (root, stats->name, new);

  g_free (first_ts);
}

static void
get_threads_json (gpointer key, gpointer value, gpointer user_data)
{
  GstThreadStats *stats = NULL;
  JsonObject *root = NULL;
  JsonObject *new = NULL;
  gchar *id = NULL;
  gchar *timestamp = NULL;

  g_return_if_fail ((GstThreadStats *) value);
  g_return_if_fail ((JsonObject *) user_data);

  stats = (GstThreadStats *) value;
  root = (JsonObject *) user_data;

  /* Create new thread object */
  new = json_object_new ();
  timestamp =
      g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (stats->tthread));
  id = g_strdup_printf ("0x%lx", (guint64) key);
  json_object_set_int_member (new, "cpuload", stats->cpuload);
  json_object_set_string_member (new, "timestamp", timestamp);

  /* Add thread object */
  json_object_set_object_member (root, id, new);

  g_free (timestamp);
  g_free (id);
}

static void
get_latencies_json (gpointer key, gpointer value, gpointer user_data)
{
  GstLatencyStats *stats = NULL;
  JsonObject *root = NULL;
  JsonObject *new = NULL;
  gchar *timestamp = NULL;
  gchar *min = NULL;
  gchar *max = NULL;
  gchar *mean = NULL;

  g_return_if_fail ((GstLatencyStats *) value);
  g_return_if_fail ((JsonObject *) user_data);

  stats = (GstLatencyStats *) value;
  root = (JsonObject *) user_data;

  /* Create new latency object */
  new = json_object_new ();
  timestamp =
      g_strdup_printf ("%" GST_TIME_FORMAT,
      GST_TIME_ARGS (stats->first_latency_ts));
  min = g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (stats->min));
  max = g_strdup_printf ("%" GST_TIME_FORMAT, GST_TIME_ARGS (stats->max));
  mean =
      g_strdup_printf ("%" GST_TIME_FORMAT,
      GST_TIME_ARGS (stats->total / stats->count));
  json_object_set_int_member (new, "buffer_count", stats->count);
  json_object_set_string_member (new, "min", min);
  json_object_set_string_member (new, "max", max);
  json_object_set_string_member (new, "mean", mean);
  json_object_set_string_member (new, "first_latency_ts", timestamp);

  /* Add latency object */
  json_object_set_object_member (root, (const gchar *) key, new);

  g_free (timestamp);
  g_free (min);
  g_free (max);
  g_free (mean);
}

static gchar *
gstd_stats_get_json (GstdStats * self)
{
  gchar *stats = NULL;
  JsonObject *stats_root, *elements, *threads, *pads, *latencies;
  JsonNode *root_node = NULL;
  JsonGenerator *generator = NULL;

  g_return_val_if_fail (self, NULL);

  stats_root = json_object_new ();

  json_object_set_int_member (stats_root, "num_messages", self->num_messages);
  json_object_set_int_member (stats_root, "num_buffers", self->num_buffers);
  json_object_set_int_member (stats_root, "num_events", self->num_events);
  json_object_set_int_member (stats_root, "num_queries", self->num_queries);
  json_object_set_int_member (stats_root, "num_elements", self->num_elements);
  json_object_set_int_member (stats_root, "num_bins", self->num_bins);
  json_object_set_int_member (stats_root, "num_pads", self->num_pads);
  json_object_set_int_member (stats_root, "num_ghostpads", self->num_ghostpads);

  if (self->have_cpuload) {
    json_object_set_int_member (stats_root, "total_cpuload",
        self->total_cpuload);
  }

  /* Add elements info */
  elements = json_object_new ();
  g_ptr_array_foreach (self->elements, get_elements_json, elements);
  json_object_set_object_member (stats_root, "elements", elements);

  /* Add pads info */
  pads = json_object_new ();
  g_ptr_array_foreach (self->pads, get_pads_json, pads);
  json_object_set_object_member (stats_root, "pads", pads);

  /* Add threads info */
  threads = json_object_new ();
  g_hash_table_foreach (self->threads, get_threads_json, threads);
  json_object_set_object_member (stats_root, "threads", threads);

  /* Add latency info */
  latencies = json_object_new ();
  g_hash_table_foreach (self->latencies, get_latencies_json, latencies);
  g_hash_table_foreach (self->element_latencies, get_latencies_json, latencies);
  /* TODO: Add element-reported-latencies */
  json_object_set_object_member (stats_root, "latencies", latencies);

  root_node = json_node_new (JSON_NODE_OBJECT);
  json_node_set_object (root_node, stats_root);

  generator = json_generator_new ();
  json_generator_set_root (generator, root_node);
  stats = json_generator_to_data (generator, NULL);

  g_object_unref (generator);
  json_node_unref (root_node);
  json_object_unref (stats_root);

  return stats;
}

/* Stats parsing methods */

static inline GstElementStats *
get_element_stats (GstdStats * self, guint ix)
{
  GPtrArray *elements = NULL;

  g_return_val_if_fail (self, NULL);

  elements = self->elements;

  return (ix != G_MAXUINT && ix < elements->len) ?
      g_ptr_array_index (elements, ix) : NULL;
}

static void
do_message_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts;
  guint elem_ix;
  GstElementStats *elem_stats;

  g_return_if_fail (self);
  g_return_if_fail (s);

  self->num_messages++;
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "element-ix", G_TYPE_UINT, &elem_ix, NULL);
  self->last_ts = MAX (self->last_ts, ts);
  if (!(elem_stats = get_element_stats (self, elem_ix))) {
    GST_WARNING ("no element stats found for ix=%u", elem_ix);
    return;
  }
  elem_stats->num_messages++;
}

static inline GstPadStats *
get_pad_stats (GstdStats * self, guint ix)
{
  GPtrArray *pads = NULL;

  g_return_val_if_fail (self, NULL);

  pads = self->pads;

  return (ix != G_MAXUINT && ix < pads->len) ?
      g_ptr_array_index (pads, ix) : NULL;
}

static void
do_event_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts;
  guint pad_ix, elem_ix;
  GstPadStats *pad_stats;
  GstElementStats *elem_stats;

  g_return_if_fail (self);
  g_return_if_fail (s);

  self->num_events++;
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "pad-ix", G_TYPE_UINT, &pad_ix, "element-ix", G_TYPE_UINT, &elem_ix,
      NULL);
  self->last_ts = MAX (self->last_ts, ts);
  if (!(pad_stats = get_pad_stats (self, pad_ix))) {
    GST_WARNING ("no pad stats found for ix=%u", pad_ix);
    return;
  }
  if (!(elem_stats = get_element_stats (self, elem_ix))) {
    // e.g. reconfigure events are send over unparented pads
    GST_INFO ("no element stats found for ix=%u", elem_ix);
    return;
  }
  elem_stats->num_events++;
}

static void
new_pad_stats (GstdStats * self, GstStructure * s)
{
  GPtrArray *pads;
  GstPadStats *stats;
  guint ix, parent_ix;
  gchar *type, *name;
  gboolean is_ghost_pad;
  GstPadDirection dir;
  guint64 thread_id;

  g_return_if_fail (self);
  g_return_if_fail (s);

  pads = self->pads;

  gst_structure_get (s,
      "ix", G_TYPE_UINT, &ix,
      "parent-ix", G_TYPE_UINT, &parent_ix,
      "name", G_TYPE_STRING, &name,
      "type", G_TYPE_STRING, &type,
      "is-ghostpad", G_TYPE_BOOLEAN, &is_ghost_pad,
      "pad-direction", GST_TYPE_PAD_DIRECTION, &dir,
      "thread-id", G_TYPE_UINT64, &thread_id, NULL);

  stats = g_slice_new0 (GstPadStats);
  if (is_ghost_pad)
    self->num_ghostpads++;
  self->num_pads++;
  stats->name = name;
  stats->type_name = type;
  stats->index = ix;
  stats->is_ghost_pad = is_ghost_pad;
  stats->dir = dir;
  stats->min_size = G_MAXUINT;
  stats->first_ts = stats->last_ts = stats->next_ts = GST_CLOCK_TIME_NONE;
  stats->thread_id = (gpointer) (guintptr) thread_id;
  stats->parent_ix = parent_ix;

  if (pads->len <= ix)
    g_ptr_array_set_size (pads, ix + 1);
  g_ptr_array_index (pads, ix) = stats;
}

static void
new_element_stats (GstdStats * self, GstStructure * s)
{
  GPtrArray *elements;
  GstElementStats *stats;
  guint ix, parent_ix;
  gchar *type, *name;
  gboolean is_bin;

  g_return_if_fail (self);
  g_return_if_fail (s);

  elements = self->elements;

  gst_structure_get (s,
      "ix", G_TYPE_UINT, &ix,
      "parent-ix", G_TYPE_UINT, &parent_ix,
      "name", G_TYPE_STRING, &name,
      "type", G_TYPE_STRING, &type, "is-bin", G_TYPE_BOOLEAN, &is_bin, NULL);

  stats = g_slice_new0 (GstElementStats);
  if (is_bin)
    self->num_bins++;
  self->num_elements++;
  stats->index = ix;
  stats->name = name;
  stats->type_name = type;
  stats->is_bin = is_bin;
  stats->first_ts = GST_CLOCK_TIME_NONE;
  stats->parent_ix = parent_ix;

  if (elements->len <= ix)
    g_ptr_array_set_size (elements, ix + 1);
  g_ptr_array_index (elements, ix) = stats;
}

static inline GstThreadStats *
get_thread_stats (GstdStats * self, gpointer id)
{
  GstThreadStats *stats = NULL;

  g_return_val_if_fail (self, NULL);

  stats = g_hash_table_lookup (self->threads, id);

  if (G_UNLIKELY (!stats)) {
    stats = g_slice_new0 (GstThreadStats);
    stats->tthread = GST_CLOCK_TIME_NONE;
    g_hash_table_insert (self->threads, id, stats);
  }
  return stats;
}

static void
do_pad_stats (GstdStats * self, GstPadStats * stats, guint elem_ix, guint size,
    guint64 ts, guint64 buffer_ts, guint64 buffer_dur,
    GstBufferFlags buffer_flags)
{
  gulong avg_size;

  g_return_if_fail (self);
  g_return_if_fail (stats);

  /* parentage */
  if (stats->parent_ix == G_MAXUINT) {
    stats->parent_ix = elem_ix;
  }

  if (stats->thread_id) {
    get_thread_stats (self, stats->thread_id);
  }

  /* size stats */
  avg_size = (((gulong) stats->avg_size * (gulong) stats->num_buffers) + size);
  stats->num_buffers++;
  stats->avg_size = (guint) (avg_size / stats->num_buffers);
  if (size < stats->min_size)
    stats->min_size = size;
  else if (size > stats->max_size)
    stats->max_size = size;
  /* time stats */
  if (!GST_CLOCK_TIME_IS_VALID (stats->last_ts))
    stats->first_ts = ts;
  stats->last_ts = ts;
  /* flag stats */
  if (buffer_flags & GST_BUFFER_FLAG_LIVE)
    stats->num_live++;
  if (buffer_flags & GST_BUFFER_FLAG_DECODE_ONLY)
    stats->num_decode_only++;
  if (buffer_flags & GST_BUFFER_FLAG_DISCONT)
    stats->num_discont++;
  if (buffer_flags & GST_BUFFER_FLAG_RESYNC)
    stats->num_resync++;
  if (buffer_flags & GST_BUFFER_FLAG_CORRUPTED)
    stats->num_corrupted++;
  if (buffer_flags & GST_BUFFER_FLAG_MARKER)
    stats->num_marker++;
  if (buffer_flags & GST_BUFFER_FLAG_HEADER)
    stats->num_header++;
  if (buffer_flags & GST_BUFFER_FLAG_GAP)
    stats->num_gap++;
  if (buffer_flags & GST_BUFFER_FLAG_DROPPABLE)
    stats->num_droppable++;
  if (buffer_flags & GST_BUFFER_FLAG_DELTA_UNIT)
    stats->num_delta++;
  /* update timestamps */
  if (GST_CLOCK_TIME_IS_VALID (buffer_ts) &&
      GST_CLOCK_TIME_IS_VALID (buffer_dur)) {
    stats->next_ts = buffer_ts + buffer_dur;
  } else {
    stats->next_ts = GST_CLOCK_TIME_NONE;
  }
}

static void
do_element_stats (GstdStats * self, GstElementStats * stats,
    GstElementStats * peer_stats, guint size, guint64 ts)
{
  g_return_if_fail (self);
  g_return_if_fail (stats);
  g_return_if_fail (peer_stats);

  stats->sent_buffers++;
  peer_stats->recv_buffers++;
  stats->sent_bytes += size;
  peer_stats->recv_bytes += size;
  /* time stats */
  if (G_UNLIKELY (!GST_CLOCK_TIME_IS_VALID (stats->first_ts))) {
    stats->first_ts = ts;
  }
  if (G_UNLIKELY (!GST_CLOCK_TIME_IS_VALID (peer_stats->first_ts))) {
    peer_stats->first_ts = ts + 1;
  }
}

static void
do_buffer_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts;
  guint64 buffer_pts = GST_CLOCK_TIME_NONE, buffer_dur = GST_CLOCK_TIME_NONE;
  guint pad_ix, elem_ix, peer_elem_ix;
  guint size;
  GstBufferFlags buffer_flags;
  GstPadStats *pad_stats;
  GstElementStats *elem_stats = NULL, *peer_elem_stats = NULL;

  g_return_if_fail (self);
  g_return_if_fail (s);

  self->num_buffers++;
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "pad-ix", G_TYPE_UINT, &pad_ix,
      "element-ix", G_TYPE_UINT, &elem_ix,
      "peer-element-ix", G_TYPE_UINT, &peer_elem_ix,
      "buffer-size", G_TYPE_UINT, &size,
      "buffer-flags", GST_TYPE_BUFFER_FLAGS, &buffer_flags, NULL);
  gst_structure_get_uint64 (s, "buffer-pts", &buffer_pts);
  gst_structure_get_uint64 (s, "buffer-duration", &buffer_dur);
  self->last_ts = MAX (self->last_ts, ts);
  if (!(pad_stats = get_pad_stats (self, pad_ix))) {
    GST_WARNING ("no pad stats found for ix=%u", pad_ix);
    return;
  }
  if (!(elem_stats = get_element_stats (self, elem_ix))) {
    GST_WARNING ("no element stats found for ix=%u", elem_ix);
    return;
  }
  if (!(peer_elem_stats = get_element_stats (self, peer_elem_ix))) {
    GST_WARNING ("no element stats found for ix=%u", peer_elem_ix);
    return;
  }
  do_pad_stats (self, pad_stats, elem_ix, size, ts, buffer_pts, buffer_dur,
      buffer_flags);
  if (pad_stats->dir == GST_PAD_SRC) {
    /* push */
    do_element_stats (self, elem_stats, peer_elem_stats, size, ts);
  } else {
    /* pull */
    do_element_stats (self, peer_elem_stats, elem_stats, size, ts);
  }
}

static void
do_query_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts;
  guint elem_ix;
  GstElementStats *elem_stats = NULL;

  g_return_if_fail (self);
  g_return_if_fail (s);

  self->num_queries++;
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "element-ix", G_TYPE_UINT, &elem_ix, NULL);
  self->last_ts = MAX (self->last_ts, ts);
  if (!(elem_stats = get_element_stats (self, elem_ix))) {
    GST_WARNING ("no element stats found for ix=%u", elem_ix);
    return;
  }
  elem_stats->num_queries++;
}

static void
do_thread_rusage_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts, tthread, thread_id;
  guint cpuload;
  GstThreadStats *thread_stats = NULL;

  g_return_if_fail (self);
  g_return_if_fail (s);

  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "thread-id", G_TYPE_UINT64, &thread_id,
      "average-cpuload", G_TYPE_UINT, &cpuload, "time", G_TYPE_UINT64, &tthread,
      NULL);
  thread_stats = get_thread_stats (self, (gpointer) (guintptr) thread_id);
  thread_stats->cpuload = cpuload;
  thread_stats->tthread = tthread;
  self->last_ts = MAX (self->last_ts, ts);
}

static void
do_proc_rusage_stats (GstdStats * self, GstStructure * s)
{
  guint64 ts;

  g_return_if_fail (self);
  g_return_if_fail (s);

  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts,
      "average-cpuload", G_TYPE_UINT, &self->total_cpuload, NULL);
  self->last_ts = MAX (self->last_ts, ts);
  self->have_cpuload = TRUE;
}

static void
update_latency_table (GstdStats * self, GHashTable * table, const gchar * key,
    guint64 time, GstClockTime ts)
{
  GstLatencyStats *ls = NULL;

  g_return_if_fail (self);
  g_return_if_fail (table);
  g_return_if_fail (key);

  /* Find the values in the hash table */
  ls = g_hash_table_lookup (table, key);
  if (!ls) {
    /* Insert the new key if the value does not exist */
    ls = g_new0 (GstLatencyStats, 1);
    ls->name = g_strdup (key);
    ls->count = 1;
    ls->total = time;
    ls->min = time;
    ls->max = time;
    ls->first_latency_ts = ts;
    g_hash_table_insert (table, g_strdup (key), ls);
  } else {
    /* Otherwise update the existing value */
    ls->count++;
    ls->total += time;
    if (ls->min > time)
      ls->min = time;
    if (ls->max < time)
      ls->max = time;
  }
}

static void
do_latency_stats (GstdStats * self, GstStructure * s)
{
  gchar *key = NULL;
  const gchar *src = NULL, *sink = NULL, *src_element = NULL,
      *sink_element = NULL, *src_element_id = NULL, *sink_element_id = NULL;
  guint64 ts = 0, time = 0;

  g_return_if_fail (self);
  g_return_if_fail (s);

  /* Get the values from the structure */
  src = gst_structure_get_string (s, "src");
  sink = gst_structure_get_string (s, "sink");
  src_element = gst_structure_get_string (s, "src-element");
  sink_element = gst_structure_get_string (s, "sink-element");
  src_element_id = gst_structure_get_string (s, "src-element-id");
  sink_element_id = gst_structure_get_string (s, "sink-element-id");
  gst_structure_get (s, "time", G_TYPE_UINT64, &time, NULL);
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts, NULL);

  /* Update last_ts */
  self->last_ts = MAX (self->last_ts, ts);

  /* Get the key */
  key = g_strdup_printf ("%s.%s.%s|%s.%s.%s", src_element_id, src_element,
      src, sink_element_id, sink_element, sink);

  /* Update the latency in the table */
  update_latency_table (self, self->latencies, key, time, ts);

  /* Clean up */
  g_free (key);

  self->have_latency = TRUE;
}

static void
do_element_latency_stats (GstdStats * self, GstStructure * s)
{
  gchar *key = NULL;
  const gchar *src = NULL, *element = NULL, *element_id = NULL;
  guint64 ts = 0, time = 0;

  g_return_if_fail (self);
  g_return_if_fail (s);

  /* Get the values from the structure */
  src = gst_structure_get_string (s, "src");
  element = gst_structure_get_string (s, "element");
  element_id = gst_structure_get_string (s, "element-id");
  gst_structure_get (s, "time", G_TYPE_UINT64, &time, NULL);
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts, NULL);

  /* Update last_ts */
  self->last_ts = MAX (self->last_ts, ts);

  /* Get the key */
  key = g_strdup_printf ("%s.%s.%s", element_id, element, src);

  /* Update the latency in the table */
  update_latency_table (self, self->element_latencies, key, time, ts);

  /* Clean up */
  g_free (key);

  self->have_element_latency = TRUE;
}

static void
do_element_reported_latency (GstdStats * self, GstStructure * s)
{
  const gchar *element = NULL, *element_id = NULL;
  guint64 ts = 0, min = 0, max = 0;
  GstReportedLatency *rl = NULL;

  g_return_if_fail (self);
  g_return_if_fail (s);

  /* Get the values from the structure */
  element_id = gst_structure_get_string (s, "element-id");
  element = gst_structure_get_string (s, "element");
  gst_structure_get (s, "min", G_TYPE_UINT64, &min, NULL);
  gst_structure_get (s, "max", G_TYPE_UINT64, &max, NULL);
  gst_structure_get (s, "ts", G_TYPE_UINT64, &ts, NULL);

  /* Update last_ts */
  self->last_ts = MAX (self->last_ts, ts);

  /* Insert/Update the key in the table */
  rl = g_new0 (GstReportedLatency, 1);
  rl->element = g_strdup_printf ("%s.%s", element_id, element);
  rl->ts = ts;
  rl->min = min;
  rl->max = max;
  g_queue_push_tail (self->element_reported_latencies, rl);

  self->have_element_reported_latency = TRUE;
}

static void
gstd_stats_collect_stats (GstdStats * self, GstDebugMessage * message)
{
  const gchar *data = NULL;
  GstStructure *s = NULL;

  g_return_if_fail (self);
  g_return_if_fail (message);

  data = gst_debug_message_get (message);
  if ((s = gst_structure_from_string (data, NULL))) {
    const gchar *name = gst_structure_get_name (s);
    if (!strcmp (name, "new-pad")) {
      new_pad_stats (self, s);
    } else if (!strcmp (name, "new-element")) {
      new_element_stats (self, s);
    } else if (!strcmp (name, "buffer")) {
      do_buffer_stats (self, s);
    } else if (!strcmp (name, "event")) {
      do_event_stats (self, s);
    } else if (!strcmp (name, "message")) {
      do_message_stats (self, s);
    } else if (!strcmp (name, "query")) {
      do_query_stats (self, s);
    } else if (!strcmp (name, "thread-rusage")) {
      do_thread_rusage_stats (self, s);
    } else if (!strcmp (name, "proc-rusage")) {
      do_proc_rusage_stats (self, s);
    } else if (!strcmp (name, "latency")) {
      do_latency_stats (self, s);
    } else if (!strcmp (name, "element-latency")) {
      do_element_latency_stats (self, s);
    } else if (!strcmp (name, "element-reported-latency")) {
      do_element_reported_latency (self, s);
    } else {
      /* TODO(ensonic): parse the xxx.class log lines */
      if (!g_str_has_suffix (data, ".class")) {
        GST_WARNING ("unknown log entry: '%s'", data);
      }
    }
    gst_structure_free (s);
  } else {
    GST_WARNING ("unknown log entry: '%s'", data);
  }
}

static void
gstd_stats_log_monitor (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line, GObject * object,
    GstDebugMessage * message, gpointer user_data)
{
  GstdStats *self = NULL;

  g_return_if_fail (GSTD_STATS (user_data));
  g_return_if_fail (message);

  self = GSTD_STATS (user_data);

  if (self->enable) {
    if (GST_LEVEL_TRACE == level) {
      gstd_stats_collect_stats (self, message);
    }
  }
}

/* Free methods */

static void
free_latency_stats (gpointer data)
{
  GstLatencyStats *ls = data;

  g_free (ls->name);
  g_slice_free (GstLatencyStats, data);
}

static void
free_reported_latency (gpointer data)
{
  GstReportedLatency *rl = data;

  if (rl->element)
    g_free (rl->element);

  g_free (data);
}

static void
free_element_stats (gpointer data)
{
  g_slice_free (GstElementStats, data);
}

static void
free_pad_stats (gpointer data)
{
  g_slice_free (GstPadStats, data);
}

static void
free_thread_stats (gpointer data)
{
  g_slice_free (GstThreadStats, data);
}
