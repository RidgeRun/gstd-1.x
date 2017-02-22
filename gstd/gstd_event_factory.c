/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2016 RidgeRun Engineering <support@ridgerun.com>
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

#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include "gstd_event_factory.h"

#define GSTD_EVENT_FACTORY_SEEK_RATE_DEFAULT 1.0
#define GSTD_EVENT_FACTORY_SEEK_FORMAT_DEFAULT GST_FORMAT_TIME
#define GSTD_EVENT_FACTORY_SEEK_FLAGS_DEFAULT GST_SEEK_FLAG_FLUSH
#define GSTD_EVENT_FACTORY_SEEK_RATE_START_TYPE_DEFAULT GST_SEEK_TYPE_SET
#define GSTD_EVENT_FACTORY_SEEK_START_DEFAULT 1*GST_SECOND
#define GSTD_EVENT_FACTORY_SEEK_STOP_TYPE_DEFAULT GST_SEEK_TYPE_SET
#define GSTD_EVENT_FACTORY_SEEK_STOP_DEFAULT 0


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

GstdEventType gstd_event_factory_parse_event (gchar *);

GstEvent * gstd_event_factory_make (gchar *name, gchar *description){

  GstEvent *event = NULL;
  GstdEventType type = gstd_event_factory_parse_event(name);
  gdouble rate = GSTD_EVENT_FACTORY_SEEK_RATE_DEFAULT;
  GstFormat format = GSTD_EVENT_FACTORY_SEEK_FORMAT_DEFAULT;
  GstSeekFlags flags = GSTD_EVENT_FACTORY_SEEK_FLAGS_DEFAULT;
  GstSeekType start_type = GSTD_EVENT_FACTORY_SEEK_RATE_START_TYPE_DEFAULT;
  gint64 start = GSTD_EVENT_FACTORY_SEEK_START_DEFAULT;
  GstSeekType stop_type = GSTD_EVENT_FACTORY_SEEK_STOP_TYPE_DEFAULT;
  gint64 stop = GSTD_EVENT_FACTORY_SEEK_STOP_DEFAULT;
  
  switch(type){
  case GSTD_EVENT_EOS:
    event = gst_event_new_eos ();
    break; 
  case GSTD_EVENT_SEEK:
    event = gst_event_new_seek (rate,format,flags,start_type,start,stop_type,stop);
    break;
  default:
    break;
  }
  
  return event;
}

GstdEventType 
gstd_event_factory_parse_event (gchar * name){
  GstdEventType ret = GSTD_EVENT_UNKNOWN;
  if(!strncmp(name,"eos",5)){
    ret = GSTD_EVENT_EOS;
  }
  else if (!strncmp(name,"seek",6)){
    ret = GSTD_EVENT_SEEK;
  }
  return ret;
}
