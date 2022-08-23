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

#include <gst/gst.h>
#include "gstd_msg_type.h"

GType
gstd_msg_type_get_type (void)
{
  static gsize gstd_msg_type_type = 0;
  static const GFlagsValue gstd_msg_type[] = {
    {GST_MESSAGE_UNKNOWN, "GST_MESSAGE_UNKNOWN", "unknown"},
    {GST_MESSAGE_UNKNOWN, "GST_MESSAGE_UNKNOWN", "none"},
    {GST_MESSAGE_EOS, "GST_MESSAGE_EOS", "eos"},
    {GST_MESSAGE_ERROR, "GST_MESSAGE_ERROR", "error"},
    {GST_MESSAGE_WARNING, "GST_MESSAGE_WARNING", "warning"},
    {GST_MESSAGE_INFO, "GST_MESSAGE_INFO", "info"},
    {GST_MESSAGE_TAG, "GST_MESSAGE_TAG", "tag"},
    {GST_MESSAGE_BUFFERING, "GST_MESSAGE_BUFFERING", "buffering"},
    {GST_MESSAGE_STATE_CHANGED, "GST_MESSAGE_STATE_CHANGED", "state_changed"},
    {GST_MESSAGE_STATE_CHANGED, "GST_MESSAGE_STATE_CHANGED", "state-changed"},
    {GST_MESSAGE_STATE_DIRTY, "GST_MESSAGE_STATE_DIRTY", "state_dirty"},
    {GST_MESSAGE_STATE_DIRTY, "GST_MESSAGE_STATE_DIRTY", "state-dirty"},
    {GST_MESSAGE_STEP_DONE, "GST_MESSAGE_STEP_DONE", "step_done"},
    {GST_MESSAGE_STEP_DONE, "GST_MESSAGE_STEP_DONE", "step-done"},
    {GST_MESSAGE_CLOCK_PROVIDE, "GST_MESSAGE_CLOCK_PROVIDE", "clock_provide"},
    {GST_MESSAGE_CLOCK_PROVIDE, "GST_MESSAGE_CLOCK_PROVIDE", "clock-provide"},
    {GST_MESSAGE_CLOCK_LOST, "GST_MESSAGE_CLOCK_LOST", "clock_lost"},
    {GST_MESSAGE_CLOCK_LOST, "GST_MESSAGE_CLOCK_LOST", "clock-lost"},
    {GST_MESSAGE_NEW_CLOCK, "GST_MESSAGE_NEW_CLOCK", "new_clock"},
    {GST_MESSAGE_NEW_CLOCK, "GST_MESSAGE_NEW_CLOCK", "new-clock"},
    {GST_MESSAGE_STRUCTURE_CHANGE, "GST_MESSAGE_STRUCTURE_CHANGE",
        "structure_change"},
    {GST_MESSAGE_STRUCTURE_CHANGE, "GST_MESSAGE_STRUCTURE_CHANGE",
        "structure-change"},
    {GST_MESSAGE_STREAM_STATUS, "GST_MESSAGE_STREAM_STATUS", "stream_status"},
    {GST_MESSAGE_STREAM_STATUS, "GST_MESSAGE_STREAM_STATUS", "stream-status"},
    {GST_MESSAGE_APPLICATION, "GST_MESSAGE_APPLICATION", "application"},
    {GST_MESSAGE_ELEMENT, "GST_MESSAGE_ELEMENT", "element"},
    {GST_MESSAGE_SEGMENT_START, "GST_MESSAGE_SEGMENT_START", "segment_start"},
    {GST_MESSAGE_SEGMENT_START, "GST_MESSAGE_SEGMENT_START", "segment-start"},
    {GST_MESSAGE_SEGMENT_DONE, "GST_MESSAGE_SEGMENT_DONE", "segment_done"},
    {GST_MESSAGE_SEGMENT_DONE, "GST_MESSAGE_SEGMENT_DONE", "segment-done"},
    {GST_MESSAGE_DURATION_CHANGED, "GST_MESSAGE_DURATION_CHANGED",
        "duration_changed"},
    {GST_MESSAGE_DURATION_CHANGED, "GST_MESSAGE_DURATION_CHANGED",
        "duration-changed"},
    {GST_MESSAGE_LATENCY, "GST_MESSAGE_LATENCY", "latency"},
    {GST_MESSAGE_ASYNC_START, "GST_MESSAGE_ASYNC_START", "async_start"},
    {GST_MESSAGE_ASYNC_START, "GST_MESSAGE_ASYNC_START", "async-start"},
    {GST_MESSAGE_ASYNC_DONE, "GST_MESSAGE_ASYNC_DONE", "async_done"},
    {GST_MESSAGE_ASYNC_DONE, "GST_MESSAGE_ASYNC_DONE", "async-done"},
    {GST_MESSAGE_REQUEST_STATE, "GST_MESSAGE_REQUEST_STATE", "request_state"},
    {GST_MESSAGE_REQUEST_STATE, "GST_MESSAGE_REQUEST_STATE", "request-state"},
    {GST_MESSAGE_STEP_START, "GST_MESSAGE_STEP_START", "step_start"},
    {GST_MESSAGE_STEP_START, "GST_MESSAGE_STEP_START", "step-start"},
    {GST_MESSAGE_QOS, "GST_MESSAGE_QOS", "qos"},
    {GST_MESSAGE_PROGRESS, "GST_MESSAGE_PROGRESS", "progress"},
    {GST_MESSAGE_TOC, "GST_MESSAGE_TOC", "toc"},
    {GST_MESSAGE_RESET_TIME, "GST_MESSAGE_RESET_TIME", "reset_time"},
    {GST_MESSAGE_RESET_TIME, "GST_MESSAGE_RESET_TIME", "reset-time"},
    {GST_MESSAGE_STREAM_START, "GST_MESSAGE_STREAM_START", "stream_start"},
    {GST_MESSAGE_STREAM_START, "GST_MESSAGE_STREAM_START", "stream-start"},
#if GST_VERSION_MINOR >= 2
    {GST_MESSAGE_NEED_CONTEXT, "GST_MESSAGE_NEED_CONTEXT", "need_context"},
    {GST_MESSAGE_NEED_CONTEXT, "GST_MESSAGE_NEED_CONTEXT", "need-context"},
    {GST_MESSAGE_HAVE_CONTEXT, "GST_MESSAGE_HAVE_CONTEXT", "have_context"},
    {GST_MESSAGE_HAVE_CONTEXT, "GST_MESSAGE_HAVE_CONTEXT", "have-context"},
#endif
#if GST_VERSION_MINOR >= 4
    {GST_MESSAGE_EXTENDED, "GST_MESSAGE_EXTENDED", "extended"},
    {GST_MESSAGE_DEVICE_ADDED, "GST_MESSAGE_DEVICE_ADDED", "device_added"},
    {GST_MESSAGE_DEVICE_ADDED, "GST_MESSAGE_DEVICE_ADDED", "device-added"},
    {GST_MESSAGE_DEVICE_REMOVED, "GST_MESSAGE_DEVICE_REMOVED",
        "device_removed"},
    {GST_MESSAGE_DEVICE_REMOVED, "GST_MESSAGE_DEVICE_REMOVED",
        "device-removed"},
#endif
#if GST_VERSION_MINOR >= 10
    {GST_MESSAGE_PROPERTY_NOTIFY, "GST_MESSAGE_PROPERTY_NOTIFY",
        "property_notify"},
    {GST_MESSAGE_PROPERTY_NOTIFY, "GST_MESSAGE_PROPERTY_NOTIFY",
        "property-notify"},
    {GST_MESSAGE_STREAM_COLLECTION, "GST_MESSAGE_STREAM_COLLECTION",
        "stream_collection"},
    {GST_MESSAGE_STREAM_COLLECTION, "GST_MESSAGE_STREAM_COLLECTION",
        "stream-collection"},
    {GST_MESSAGE_STREAMS_SELECTED, "GST_MESSAGE_STREAMS_SELECTED",
        "streams_selected"},
    {GST_MESSAGE_STREAMS_SELECTED, "GST_MESSAGE_STREAMS_SELECTED",
        "streams-selected"},
    {GST_MESSAGE_REDIRECT, "GST_MESSAGE_REDIRECT", "redirect"},
#endif
    {GST_MESSAGE_ANY, "GST_MESSAGE_ANY", "any"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&gstd_msg_type_type)) {
    GType tmp = g_flags_register_static ("GstdMsgType", gstd_msg_type);
    g_once_init_leave (&gstd_msg_type_type, tmp);
  }

  return (GType) gstd_msg_type_type;
}
