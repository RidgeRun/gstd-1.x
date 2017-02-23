/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#ifndef __GSTD_EVENT_HANDLER_H__
#define __GSTD_EVENT_HANDLER_H__

#include <gst/gst.h>
#include <gstd_object.h>

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
GType gstd_event_handler_get_type ();


/**
 * gstd_event_new: (constructor)
 * @receiver: The object that will receive the event.
 *
 * Creates a new object that sends events.
 *
 * Returns: (transfer full) (nullable): A new #GstdEvent. Free after
 * usage using g_object_unref()
 */
GstdEventHandler *gstd_event_handler_new (GObject * receiver);

/**
 * gstd_event_send_event:
 * @gstd_event: The member of the corresponding element that will send a gst event.
 * @event_type: Event type that will be sent. 
 * @description: (nullable) Parameters of the event_type.
 * 
 * Sends the specified event to the receiver object.
 *
 * Returns: TRUE if the event is sent succesfully to the receiver. FALSE otherwise.
 */
gboolean gstd_event_handler_send_event (GstdEventHandler * gstd_event_handler,
    const gchar * event_type, const gchar * description);

G_END_DECLS
#endif // __GSTD_EVENT_HANDLER_H__
