/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2023 Ridgerun, LLC (http://www.ridgerun.com)
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

#include "gstd_stats.h"
#include "gstd_object.h"
#include "gstd_property_reader.h"

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
};

struct _GstdStatsClass
{
  GstdObjectClass parent_class;
};

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
