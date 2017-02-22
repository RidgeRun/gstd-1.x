#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_event_handler.h"

enum
{
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdEventHandler
{
  GObject parent;

  GParamFlags flags;
};

struct _GstdEventHandlerClass
{
  GObjectClass parent_class;
};

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

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_event_handler_debug, "gstdeventhandler", debug_color,
      "Gstd Event Handler category");
}

static void
gstd_event_handler_init (GstdEventHandler * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd event handler");
}

gboolean gstd_event_handler_send_event(
      GstdEventHandler *self, GstdObject *receiver, gchar *event_type, gchar *description)
{
    GST_INFO_OBJECT (self, "Event Handler sending event %s", event_type);
    return FALSE;
}