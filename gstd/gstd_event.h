#ifndef __GSTD_EVENT_H__
#define __GSTD_EVENT_H__

#include <gst/gst.h>
#include <gstd_object.h>

G_BEGIN_DECLS
#define GSTD_TYPE_EVENT \
  (gstd_event_get_type())
#define GSTD_EVENT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_EVENT,GstdEvent))
#define GSTD_EVENT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_EVENT,GstdEventClass))
#define GSTD_IS_EVENT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_EVENT))
#define GSTD_IS_EVENT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_EVENT))
#define GSTD_EVENT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_EVENT, GstdEventClass))
typedef struct _GstdEvent GstdEvent;
typedef struct _GstdEventClass GstdEventClass;
GType gstd_event_get_type ();


/**
 * gstd_event_new: (constructor)
 * 
 * Creates a new object that sends events.
 *
 * Returns: (transfer full) (nullable): A new #GstdEvent. Free after
 * usage using g_object_unref()
 */
GstdEvent *gstd_event_new ();

/**
 * gstd_event_send_event:
 * @gstd_event: The member of the corresponding element that will send a gst event.
 * @receiver: The object that will receive the event.
 * @event_type: Event type that will be sent. 
 * @description: (nullable) Parameters of the event_type.
 * 
 * Sends the specified event to the receiver.
 *
 * Returns: (transfer full) (nullable): A new #GstdEvent. Free after
 * usage using g_object_unref()
 */
gboolean gstd_event_send_event(
      GstdEvent *gstd_event, GstdObject *receiver, gchar *event_type, gchar *description);

G_END_DECLS

#endif // __GSTD_EVENT_H__
