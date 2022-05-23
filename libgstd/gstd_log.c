/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#define GSTD_DEBUG_LEVEL "WARNING"

static const gchar *gstd_log_get_gstd_default (void);
static const gchar *gstd_log_get_gst_default (void);
static gchar *gstd_log_get_filename (const gchar * filename,
    const gchar * default_filename);

static void
gstd_log_proxy (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line, GObject * object,
    GstDebugMessage * message, gpointer user_data)
    G_GNUC_NO_INSTRUMENT;

     static FILE *_gstdlog = NULL;
     static FILE *_gstlog = NULL;
     static gchar *gstd_filename;
     static gchar *gst_filename;

gboolean
gstd_log_init (const gchar * gstdfilename, const gchar * gstfilename)
{
  if (_gstdlog) {
    return TRUE;
  }

  gstd_filename =
      gstd_log_get_filename (gstdfilename, gstd_log_get_gstd_default ());

  _gstdlog = g_fopen (gstd_filename, "a+");

  if (!_gstdlog) {
    g_printerr ("Unable to open Gstd log file %s: %s\n", gstd_filename,
        g_strerror (errno));
    return FALSE;
  }

  gst_filename =
      gstd_log_get_filename (gstfilename, gstd_log_get_gst_default ());
  _gstlog = g_fopen (gst_filename, "a+");

  if (!_gstlog) {
    g_printerr ("Unable to open Gst log file %s: %s\n", gst_filename,
        g_strerror (errno));
    return FALSE;
  }

  /* Install our proxy handler */
  gst_debug_add_log_function (gstd_log_proxy, NULL, NULL);

  return TRUE;
}

void
gstd_debug_init (void)
{
  gint debug_color;
  /* Turn on up to info for gstd debug */
  gst_debug_set_threshold_from_string (GSTD_DEBUG_PREFIX "*:" GSTD_DEBUG_LEVEL,
      FALSE);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_debug, "gstd", debug_color, "Gstd category");
}

void
gstd_log_deinit (void)
{
  g_free (gstd_filename);
  g_free (gst_filename);

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

static const gchar *
gstd_log_get_gstd_default (void)
{
  return GSTD_LOG_STATE_DIR GSTD_LOG_NAME;
}

static const gchar *
gstd_log_get_gst_default (void)
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
gstd_log_get_current_gstd (void)
{
  return g_strdup (gstd_filename);
}

gchar *
gstd_log_get_current_gst (void)
{
  return g_strdup (gst_filename);
}
