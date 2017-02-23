#ifndef __GSTD_EVENT_H__
#define __GSTD_EVENT_H__

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
 * Returns: (transfer full) (nullable): A new #GstdEvent. Free after
 * usage using g_object_unref()
 */
gboolean gstd_event_handler_send_event (GstdEventHandler * gstd_event_handler,
    gchar * event_type, gchar * description);

G_END_DECLS
#endif // __GSTD_EVENT_H__
