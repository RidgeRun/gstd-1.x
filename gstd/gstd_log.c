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

static const gchar *gstd_log_get_gstd_default ();
static const gchar *gstd_log_get_gst_default ();
static gchar *gstd_log_get_filename (const gchar * filename, const gchar * default_filename);

static FILE *_gstdlog = NULL;
static FILE *_gstlog = NULL;
static gchar *gstd_filename;
static gchar *gst_filename;
static int gstd_log_init_run_once = NULL;

void
gstd_log_init (const gchar * gstdfilename, const gchar * gstfilename)
{
  gint debug_color;

  if (_gstdlog) {
    return;
  }

  gstd_filename =
      gstd_log_get_filename (gstdfilename, gstd_log_get_gstd_default ());
  _gstdlog = g_fopen (gstd_filename, "a+");

  if (!_gstdlog) {
    g_printerr ("Unable to open Gstd log file: %s\n", g_strerror (errno));
    return;
  }

  gst_filename =
      gstd_log_get_filename (gstfilename, gstd_log_get_gst_default ());
  _gstlog = g_fopen (gst_filename, "a+");

  if (!_gstlog) {
    g_printerr ("Unable to open Gst log file: %s\n", g_strerror (errno));
    return;
  }

  if (gstd_log_init_run_once) return;

  /* Turn on up to info for gstd debug */
  gst_debug_set_threshold_from_string (GSTD_DEBUG_PREFIX "*:" GSTD_DEBUG_LEVEL,
      FALSE);

  /* Install our proxy handler */
  gst_debug_add_log_function (gstd_log_proxy, NULL, NULL);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd", debug_color, "Gstd category");
  gstd_log_init_run_once = 1;
}

void
gstd_log_deinit ()
{
  g_free (gstd_filename);
  g_free (gst_filename);

  if (!_gstdlog) {
    return;
  }

  fclose (_gstdlog);
  fclose (_gstlog);

  _gstdlog = NULL;
  _gstlog = NULL;
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

static const gchar *
gstd_log_get_gstd_default ()
{
  return GSTD_LOG_STATE_DIR GSTD_LOG_NAME;
}

static const gchar *
gstd_log_get_gst_default ()
{
  return GSTD_LOG_STATE_DIR GST_LOG_NAME;
}

static gchar *
gstd_log_get_filename (const gchar * filename, const gchar * default_filename)
{
  if (filename == NULL)
    return g_strdup (default_filename);

  if (g_path_is_absolute (filename)) {
    return g_strdup (filename);
  } else {
    g_printerr
      ("WARNING: The pid filename is not absolute since default filename\n");
    return g_strdup (default_filename);;
  }
}

gchar *
gstd_log_get_current_gstd ()
{
  return g_strdup (gstd_filename);
}

gchar *
gstd_log_get_current_gst ()
{
  return g_strdup (gst_filename);
}
