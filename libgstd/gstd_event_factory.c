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

#include <string.h>
#include <errno.h>
#include <math.h>

#include "gstd_event_factory.h"

#define GSTD_EVENT_FACTORY_SEEK_RATE_DEFAULT 1.0
#define GSTD_EVENT_FACTORY_SEEK_FORMAT_DEFAULT GST_FORMAT_TIME
#define GSTD_EVENT_FACTORY_SEEK_FLAGS_DEFAULT GST_SEEK_FLAG_FLUSH
#define GSTD_EVENT_FACTORY_SEEK_START_TYPE_DEFAULT GST_SEEK_TYPE_SET
#define GSTD_EVENT_FACTORY_SEEK_START_DEFAULT 0
#define GSTD_EVENT_FACTORY_SEEK_STOP_TYPE_DEFAULT GST_SEEK_TYPE_SET
#define GSTD_EVENT_FACTORY_SEEK_STOP_DEFAULT GST_CLOCK_TIME_NONE
#define GSTD_EVENT_FACTORY_FLUSH_STOP_RESET_DEFAULT TRUE

typedef enum _GstdEventType GstdEventType;

enum _GstdEventType
{

  GSTD_EVENT_UNKNOWN = 0,

  GSTD_EVENT_FLUSH_START = 1,

  GSTD_EVENT_FLUSH_STOP = 2,

  GSTD_EVENT_STREAM_START = 3,

  GSTD_EVENT_CAPS = 4,

  GSTD_EVENT_SEGMENT = 5,

  GSTD_EVENT_TAG = 6,

  GSTD_EVENT_BUFFERSIZE = 7,

  GSTD_EVENT_SINK_MESSAGE = 8,

  GSTD_EVENT_EOS = 9,

  GSTD_EVENT_TOC = 10,

  GSTD_EVENT_SEGMENT_DONE = 11,

  GSTD_EVENT_GAP = 12,

  GSTD_EVENT_QOS = 13,

  GSTD_EVENT_SEEK = 14,

  GSTD_EVENT_NAVIGATION = 15
};

static gboolean gstd_ascii_to_gint64 (const gchar *, gint64 *);
static gboolean gstd_ascii_to_double (const gchar *, gdouble *);
static gboolean gstd_ascii_to_boolean (const gchar *, gboolean *);
GstdEventType gstd_event_factory_parse_event (const gchar *);
static GstEvent *gstd_event_factory_make_seek_event (const gchar *);
static GstEvent *gstd_event_factory_make_flush_stop_event (const gchar *);

GstEvent *
gstd_event_factory_make (const gchar * name, const gchar * description)
{

  GstEvent *event = NULL;
  GstdEventType type;

  g_return_val_if_fail (name, NULL);

  type = gstd_event_factory_parse_event (name);

  switch (type) {
    case GSTD_EVENT_EOS:
      event = gst_event_new_eos ();
      break;
    case GSTD_EVENT_SEEK:
      event = gstd_event_factory_make_seek_event (description);
      break;
    case GSTD_EVENT_FLUSH_START:
      event = gst_event_new_flush_start ();
      break;
    case GSTD_EVENT_FLUSH_STOP:
      event = gstd_event_factory_make_flush_stop_event (description);
      break;
    default:
      event = NULL;
      break;
  }

  return event;
}

static gboolean
gstd_ascii_to_gint64 (const gchar * full_string, gint64 * out_value)
{
  g_return_val_if_fail (out_value, FALSE);
  errno = 0;
  *out_value = g_ascii_strtod (full_string, NULL);
  if ((errno == ERANGE && (*out_value == LONG_MAX || *out_value == LONG_MIN))
      || (errno != 0 && *out_value == 0)) {
    return FALSE;
  }
  return TRUE;
}

static gboolean
gstd_ascii_to_double (const gchar * full_string, gdouble * out_value)
{
  g_return_val_if_fail (out_value, FALSE);
  errno = 0;
  *out_value = g_ascii_strtod (full_string, NULL);
  if ((errno == ERANGE && (*out_value == HUGE_VALF || *out_value == HUGE_VALL))
      || (errno != 0 && *out_value == 0)) {
    return FALSE;
  }
  return TRUE;
}

static gboolean
gstd_ascii_to_boolean (const gchar * full_string, gboolean * out_value)
{
  gboolean ret;

  g_return_val_if_fail (full_string, FALSE);
  g_return_val_if_fail (out_value, FALSE);
  ret = FALSE;

  if (!g_ascii_strcasecmp (full_string, "true")) {
    *out_value = TRUE;
    ret = TRUE;
  } else if (!g_ascii_strcasecmp (full_string, "false")) {
    *out_value = FALSE;
    ret = TRUE;
  }
  return ret;
}



static GstEvent *
gstd_event_factory_make_seek_event (const gchar * description)
{
  gdouble rate = GSTD_EVENT_FACTORY_SEEK_RATE_DEFAULT;
  GstFormat format = GSTD_EVENT_FACTORY_SEEK_FORMAT_DEFAULT;
  GstSeekFlags flags = GSTD_EVENT_FACTORY_SEEK_FLAGS_DEFAULT;
  GstSeekType start_type = GSTD_EVENT_FACTORY_SEEK_START_TYPE_DEFAULT;
  gint64 start = GSTD_EVENT_FACTORY_SEEK_START_DEFAULT;
  GstSeekType stop_type = GSTD_EVENT_FACTORY_SEEK_STOP_TYPE_DEFAULT;
  gint64 stop = GSTD_EVENT_FACTORY_SEEK_STOP_DEFAULT;
  GstEvent *event = NULL;
  gchar **tokens = NULL;
  gint64 temp_format;
  gint64 temp_flags;
  gint64 temp_start_type;
  gint64 temp_stop_type;

  if (NULL != description) {
    tokens = g_strsplit (description, " ", 7);
  }

  if (NULL == tokens || NULL == tokens[0]) {
    goto fallback;
  }

  if (!gstd_ascii_to_double (tokens[0], &rate)) {
    goto out;
  }

  if (NULL == tokens[1]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[1], &temp_format)) {
    goto out;
  }
  format = (GstFormat) temp_format;

  if (NULL == tokens[2]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[2], &temp_flags)) {
    goto out;
  }
  flags = (GstSeekFlags) temp_flags;

  if (NULL == tokens[3]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[3], &temp_start_type)) {
    goto out;
  }
  start_type = (GstSeekType) temp_start_type;

  if (NULL == tokens[4]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[4], &start)) {
    goto out;
  }

  if (NULL == tokens[5]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[5], &temp_stop_type)) {
    goto out;
  }
  stop_type = (GstSeekType) temp_stop_type;

  if (NULL == tokens[6]) {
    goto fallback;
  }

  if (!gstd_ascii_to_gint64 (tokens[6], &stop)) {
    goto out;
  }

fallback:
  {
    event =
        gst_event_new_seek (rate, format, flags, start_type, start, stop_type,
        stop);
  }
out:
  {
    g_strfreev (tokens);
    return event;
  }
}

static GstEvent *
gstd_event_factory_make_flush_stop_event (const gchar * description)
{
  gboolean reset_time = GSTD_EVENT_FACTORY_FLUSH_STOP_RESET_DEFAULT;

  if (NULL != description) {
    if (!gstd_ascii_to_boolean (description, &reset_time)) {
      return NULL;
    }
  }

  return gst_event_new_flush_stop (reset_time);
}

GstdEventType
gstd_event_factory_parse_event (const gchar * name)
{

  GstdEventType ret = GSTD_EVENT_UNKNOWN;

  g_return_val_if_fail (name, GSTD_EVENT_UNKNOWN);

  if (!strcmp (name, "eos")) {
    ret = GSTD_EVENT_EOS;
  } else if (!strcmp (name, "seek")) {
    ret = GSTD_EVENT_SEEK;
  } else if (!strcmp (name, "flush-start") || !strcmp (name, "flush_start")) {
    ret = GSTD_EVENT_FLUSH_START;
  } else if (!strcmp (name, "flush-stop") || !strcmp (name, "flush_stop")) {
    ret = GSTD_EVENT_FLUSH_STOP;
  }
  return ret;
}
