#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_event_handler.h"
#include "gstd_event_factory.h"


enum
{
  PROP_RECEIVER = 1,
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdEventHandler
{
  GObject parent;

  GParamFlags flags;

  GObject *receiver;
};

struct _GstdEventHandlerClass
{
  GObjectClass parent_class;
};

static void
gstd_event_handler_set_property (GObject *,
    guint, const GValue *, GParamSpec *);
G_DEFINE_TYPE (GstdEventHandler, gstd_event_handler, G_TYPE_OBJECT);

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_event_handler_debug);
#define GST_CAT_DEFAULT gstd_event_handler_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void
gstd_event_handler_class_init (GstdEventHandlerClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  object_class->set_property = gstd_event_handler_set_property;

  properties[PROP_RECEIVER] =
      g_param_spec_object ("receiver",
      "Receiver",
      "The object that will receive the event",
      G_TYPE_OBJECT,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_event_handler_debug, "gstdeventhandler",
      debug_color, "Gstd Event Handler category");
}

static void
gstd_event_handler_init (GstdEventHandler * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd event handler");
}

gboolean
gstd_event_handler_send_event (GstdEventHandler * self, gchar * event_type,
    gchar * description)
{
  GST_INFO_OBJECT (self, "Event Handler sending event %s", event_type);
  GstEvent *event = gstd_event_factory_make (event_type, description);
  return gst_element_send_event (GST_ELEMENT (self->receiver), event);
}


GstdEventHandler *
gstd_event_handler_new (GObject * receiver)
{
  return GSTD_EVENT_HANDLER (g_object_new (GSTD_TYPE_EVENT_HANDLER, "receiver",
          receiver, NULL));
}

static void
gstd_event_handler_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdEventHandler *self = GSTD_EVENT_HANDLER (object);

  switch (property_id) {
    case PROP_RECEIVER:
      self->receiver = g_value_get_object (value);
      GST_INFO_OBJECT (self, "Changed receiver to %p", self->receiver);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}
