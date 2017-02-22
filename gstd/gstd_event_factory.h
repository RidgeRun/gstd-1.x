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
#ifndef __GSTD_EVENT_FACTORY_H__
#define __GSTD_EVENT_FACTORY_H__



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

GstEvent *
gstd_event_factory_make ( GstdEventType, gchar *);



#endif //__GSTD_EVENT_FACTORY_H__





