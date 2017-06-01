/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_log.h"

#include <gst/gst.h>
#include <glib/gstdio.h>
#include <string.h>

GST_DEBUG_CATEGORY (gstd_debug);

#define GSTD_LOG_NAME "gstd.log"
#define GST_LOG_NAME "gst.log"
#define GSTD_DEBUG_PREFIX "gstd"
#define GSTD_DEBUG_LEVEL "INFO"

static void
gstd_log_proxy (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line, GObject * object,
    GstDebugMessage * message, gpointer user_data) G_GNUC_NO_INSTRUMENT;

static FILE *_gstdlog = NULL;
static FILE *_gstlog = NULL;

void
gstd_log_init ()
{
  gint debug_color;

  if (_gstdlog) {
    return;
  }

  _gstdlog = g_fopen (gstd_log_get_gstd (), "a+");
  if (!_gstdlog) {
    g_printerr ("Unable to open Gstd log file: %s\n", g_strerror (errno));
    return;
  }

  _gstlog = g_fopen (gstd_log_get_gst (), "a+");
  if (!_gstlog) {
    g_printerr ("Unable to open Gst log file: %s\n", g_strerror (errno));
    return;
  }

  /* Turn on up to info for gstd debug */
  gst_debug_set_threshold_from_string (GSTD_DEBUG_PREFIX "*:" GSTD_DEBUG_LEVEL,
      FALSE);

  /* Install our proxy handler */
  gst_debug_add_log_function (gstd_log_proxy, NULL, NULL);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd", debug_color, "Gstd category");
}

void
gstd_log_deinit ()
{
  if (!_gstdlog) {
    return;
  }

  fclose (_gstdlog);
  fclose (_gstlog);
}

static void
gstd_log_proxy (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line, GObject * object,
    GstDebugMessage * message, gpointer user_data)
{
  const gchar *cat_name;

  cat_name = gst_debug_category_get_name (category);

  /* Log every gstd trace into the gstd log file */
  if (!strncmp (cat_name, GSTD_DEBUG_PREFIX, sizeof (GSTD_DEBUG_PREFIX) - 1)) {
    /* Redirect gstd debug to its respective file */
    gst_debug_log_default (category, level, file, function, line, object,
        message, _gstdlog);
  } else {
    /* Everything else goes to the gst file */
    gst_debug_log_default (category, level, file, function, line, object,
        message, _gstlog);
  }
}

const gchar *
gstd_log_get_gstd ()
{
  return GSTD_LOG_STATE_DIR GSTD_LOG_NAME;
}

const gchar *
gstd_log_get_gst ()
{
  return GSTD_LOG_STATE_DIR GST_LOG_NAME;
}
