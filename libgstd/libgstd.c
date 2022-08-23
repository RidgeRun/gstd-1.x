/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd.h"

#include <stdarg.h>
#include <stdio.h>

#include "gstd_http.h"
#include "gstd_ipc.h"
#include "gstd_log.h"
#include "gstd_tcp.h"
#include "gstd_unix.h"

/**
 * Supported_IPCs:
 * @GSTD_IPC_TYPE_TCP: To enable TCP communication
 * @GSTD_IPC_TYPE_UNIX: To enable UNIX communication
 * @GSTD_IPC_TYPE_HTTP: To enable HTTP communication
 * IPC options for libGstD
 */
typedef enum _SupportedIpcs SupportedIpcs;

enum _SupportedIpcs
{
  GSTD_IPC_TYPE_TCP,
  GSTD_IPC_TYPE_UNIX,
  GSTD_IPC_TYPE_HTTP,
};

static GType gstd_supported_ipc_to_ipc (const SupportedIpcs code);
static void gstd_init (int argc, char *argv[]);
static void gstd_set_ipc (GstD * gstd);

struct _GstD
{
  GstdSession *session;
  GstdIpc **ipc_array;
  guint num_ipcs;
};

static GType
gstd_supported_ipc_to_ipc (const SupportedIpcs code)
{
  GType code_description[] = {
    [GSTD_IPC_TYPE_TCP] = GSTD_TYPE_TCP,
    [GSTD_IPC_TYPE_UNIX] = GSTD_TYPE_UNIX,
    [GSTD_IPC_TYPE_HTTP] = GSTD_TYPE_HTTP
  };

  const gint size = sizeof (code_description) / sizeof (gchar *);

  g_return_val_if_fail (0 <= code, GSTD_TYPE_IPC);
  g_return_val_if_fail (size > code, GSTD_TYPE_IPC);

  return code_description[code];
}

static void
gstd_init (int argc, char *argv[])
{
  gst_init (&argc, &argv);
  gstd_debug_init ();
}

static void
gstd_set_ipc (GstD * gstd)
{
  /* Array to specify gstd how many IPCs are supported, 
   * SupportedIpcs should be added to this array.
   */
  const SupportedIpcs supported_ipcs[] = {
    GSTD_IPC_TYPE_TCP,
    GSTD_IPC_TYPE_UNIX,
    GSTD_IPC_TYPE_HTTP,
  };

  const guint num_ipcs = (sizeof (supported_ipcs) / sizeof (SupportedIpcs));

  GstdIpc **ipc_array = NULL;

  g_return_if_fail (NULL != gstd);

  /* If there is ipcs, then initialize them */
  if (num_ipcs > 0) {
    ipc_array = g_malloc0 (num_ipcs * sizeof (*ipc_array));
    for (gint ipc_idx = 0; ipc_idx < num_ipcs; ipc_idx++) {
      ipc_array[ipc_idx] =
          GSTD_IPC (g_object_new (gstd_supported_ipc_to_ipc (supported_ipcs
                  [ipc_idx]), NULL));
    }
    gstd->ipc_array = ipc_array;
  }

  gstd->num_ipcs = num_ipcs;
  gstd->ipc_array = ipc_array;
}

void
gstd_context_add_group (GstD * gstd, GOptionContext * context)
{
  GOptionGroup *gst_options = NULL;
  GOptionGroup **ipc_group_array = NULL;

  g_return_if_fail (NULL != gstd);
  g_return_if_fail (NULL != gstd->ipc_array);
  g_return_if_fail (NULL != context);

  gst_options = gst_init_get_option_group ();
  g_option_context_add_group (context, gst_options);

  ipc_group_array = g_malloc0 (gstd->num_ipcs * sizeof (*ipc_group_array));

  for (gint ipc_idx = 0; ipc_idx < gstd->num_ipcs; ipc_idx++) {
    gstd_ipc_get_option_group (gstd->ipc_array[ipc_idx],
        &ipc_group_array[ipc_idx]);
    g_option_context_add_group (context, ipc_group_array[ipc_idx]);
  }

  g_free (ipc_group_array);
}

GstdReturnCode
gstd_new (GstD ** out, int argc, char *argv[])
{
  GstdReturnCode ret = GSTD_EOK;
  GstD *gstd = NULL;
  GstdSession *session = NULL;

  g_return_val_if_fail (NULL != out, GSTD_NULL_ARGUMENT);

  gstd = (GstD *) g_malloc0 (sizeof (*gstd));
  session = gstd_session_new ("Session0");

  gstd->session = session;
  gstd->num_ipcs = 0;
  gstd->ipc_array = NULL;

  gstd_set_ipc (gstd);

  *out = gstd;

  /* Initialize GStreamer */
  gstd_init (argc, argv);

  return ret;
}

gboolean
gstd_start (GstD * gstd)
{
  gboolean ipc_selected = FALSE;
  gboolean ret = TRUE;
  GstdReturnCode code = GSTD_EOK;
  gint ipc_idx;

  g_return_val_if_fail (NULL != gstd, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->ipc_array, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->session, GSTD_NULL_ARGUMENT);

  /* Verify if at least one IPC mechanism was selected */
  for (ipc_idx = 0; ipc_idx < gstd->num_ipcs; ipc_idx++) {
    g_object_get (G_OBJECT (gstd->ipc_array[ipc_idx]), "enabled",
        &ipc_selected, NULL);

    if (ipc_selected) {
      break;
    }
  }

  /* If no IPC was selected, default to TCP */
  if (!ipc_selected) {
    g_object_set (G_OBJECT (gstd->ipc_array[0]), "enabled", TRUE, NULL);
  }

  /* Run start for each IPC (each start method checks for the enabled flag) */
  for (ipc_idx = 0; ipc_idx < gstd->num_ipcs; ipc_idx++) {
    code = gstd_ipc_start (gstd->ipc_array[ipc_idx], gstd->session);
    if (code) {
      g_printerr ("Couldn't start IPC : (%s)\n",
          G_OBJECT_TYPE_NAME (gstd->ipc_array[ipc_idx]));
      ret = FALSE;
    }
  }

  return ret;
}

void
gstd_stop (GstD * gstd)
{
  g_return_if_fail (NULL != gstd);
  g_return_if_fail (NULL != gstd->ipc_array);
  g_return_if_fail (NULL != gstd->session);

  /* Run stop for each IPC */
  for (gint ipc_idx = 0; ipc_idx < gstd->num_ipcs; ipc_idx++) {
    if (NULL != gstd->ipc_array[ipc_idx]) {
      gstd_ipc_stop (gstd->ipc_array[ipc_idx]);
      g_clear_object (&gstd->ipc_array[ipc_idx]);
    }
  }
}

void
gstd_free (GstD * gstd)
{
  g_return_if_fail (NULL != gstd);
  gstd_stop (gstd);
  g_free (gstd->ipc_array);
  g_object_unref (gstd->session);
  g_free (gstd);
}

GstdReturnCode
gstd_create (GstD * gstd, const gchar * uri, const gchar * name,
    const gchar * description)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *node = NULL;

  g_return_val_if_fail (NULL != gstd, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != uri, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != name, GSTD_NULL_ARGUMENT);

  ret = gstd_get_by_uri (gstd->session, uri, &node);
  if (GSTD_EOK != ret) {
    goto out;
  }

  ret = gstd_object_create (node, name, description);

  g_object_unref (node);

out:
  return ret;
}

GstdReturnCode
gstd_read (GstD * gstd, const gchar * uri, GstdObject ** resource)
{
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (NULL != gstd, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != uri, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != resource, GSTD_NULL_ARGUMENT);

  ret = gstd_get_by_uri (gstd->session, uri, resource);
  return ret;
}

GstdReturnCode
gstd_update (GstD * gstd, const gchar * uri, const gchar * value)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *node = NULL;

  g_return_val_if_fail (NULL != gstd, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != uri, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != value, GSTD_NULL_ARGUMENT);

  ret = gstd_get_by_uri (gstd->session, uri, &node);
  if (GSTD_EOK != ret) {
    goto out;
  }

  ret = gstd_object_update (node, value);

  g_object_unref (node);

out:
  return ret;
}

GstdReturnCode
gstd_delete (GstD * gstd, const gchar * uri, const gchar * name)
{
  GstdReturnCode ret = GSTD_EOK;
  GstdObject *node = NULL;

  g_return_val_if_fail (NULL != gstd, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != gstd->session, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != uri, GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (NULL != name, GSTD_NULL_ARGUMENT);

  ret = gstd_get_by_uri (gstd->session, uri, &node);
  if (GSTD_EOK != ret) {
    goto out;
  }

  ret = gstd_object_delete (node, name);

  g_object_unref (node);

out:
  return ret;
}
