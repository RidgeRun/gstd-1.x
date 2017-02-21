#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_event.h"

enum
{
  N_PROPERTIES                  // NOT A PROPERTY
};

struct _GstdEvent
{
  GObject parent;

  GParamFlags flags;
};

struct _GstdEventClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (GstdEvent, gstd_event, G_TYPE_OBJECT);

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_event_debug);
#define GST_CAT_DEFAULT gstd_event_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

static void
gstd_event_class_init (GstdEventClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  /* Initialize debug category with nice colors */
  guint debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_event_debug, "gstdevent", debug_color,
      "Gstd Event category");
}

static void
gstd_event_init (GstdEvent * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd event");
}

gboolean gstd_event_send_event(
      GstdEvent *self, GstdObject *receiver, gchar *event_type, gchar *description)
{
    GST_INFO_OBJECT (self, "Sending event %s", event_type);
    return FALSE;
}