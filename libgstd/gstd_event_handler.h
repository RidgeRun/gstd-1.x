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

#ifndef __GSTD_EVENT_HANDLER_H__
#define __GSTD_EVENT_HANDLER_H__

#include <gst/gst.h>

G_BEGIN_DECLS
#define GSTD_TYPE_EVENT_HANDLER \
  (gstd_event_handler_get_type())
#define GSTD_EVENT_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_EVENT_HANDLER,GstdEventHandler))
#define GSTD_EVENT_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_EVENT_HANDLER,GstdEventHandlerClass))
#define GSTD_IS_EVENT_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_EVENT_HANDLER))
#define GSTD_IS_EVENT_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_EVENT_HANDLER))
#define GSTD_EVENT_HANDLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_EVENT_HANDLER, GstdEventHandlerClass))

typedef struct _GstdEventHandler GstdEventHandler;
typedef struct _GstdEventHandlerClass GstdEventHandlerClass;

GType gstd_event_handler_get_type (void);

/**
 * gstd_event_handler_new: (constructor)
 * @receiver: The object that will receive the event.
 *
 * Creates a new object that sends events.
 *
 * Returns: (transfer full) (nullable): A new #GstdEvent. Free after
 * usage using g_object_unref()
 */
GstdEventHandler *gstd_event_handler_new (GObject * receiver);

G_END_DECLS

#endif // __GSTD_EVENT_HANDLER_H__
