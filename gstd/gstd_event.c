#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_event.h"

/* Gstd Event debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_event_debug);
#define GST_CAT_DEFAULT gstd_event_debug
#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/* VTable */
static void
gstd_event_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void 
gstd_event_get_property (GObject *, guint, GValue *, GParamSpec *);
static void gstd_event_dispose (GObject *);
static gboolean 
gstd_event_send_event(GstdEvent *, GstdObject *, gchar *, gchar *);

static void
gstd_event_class_init (GstdEventClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };

  object_class->set_property = gstd_event_set_property;
  object_class->get_property = gstd_event_get_property;
  object_class->dispose = gstd_event_dispose;

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

static gboolean (*gstd_event_send_event)(
      GstdEvent *self, GstdObject *receiver, gchar *event_type, gchar *decription)
{
    GST_INFO_OBJECT (self, "Please Re-Implement this method on subclass");
    return FALSE;
}