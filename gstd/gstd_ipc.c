/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2016 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <gst/gst.h>

#include "gstd_ipc.h"
#include "gstd_list.h"
#include "gstd_pipeline_creator.h"
#include "gstd_pipeline_deleter.h"

/* Gstd IPC debugging category */
GST_DEBUG_CATEGORY_STATIC (gstd_ipc_debug);
#define GST_CAT_DEFAULT gstd_ipc_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdIpc, gstd_ipc, GSTD_TYPE_OBJECT);

enum
{
  N_PROPERTIES                  // NOT A PROPERTY
};


/* VTable */

static void
gstd_ipc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);

static void gstd_ipc_get_property (GObject *, guint, GValue *, GParamSpec *);


static void gstd_ipc_dispose (GObject * object);

static void
gstd_ipc_class_init (GstdIpcClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  guint debug_color;

  object_class->set_property = gstd_ipc_set_property;
  object_class->get_property = gstd_ipc_get_property;
  object_class->dispose = gstd_ipc_dispose;

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_ipc_debug, "gstdIPC", debug_color,
      "Gstd IPC category");
}

static void
gstd_ipc_init (GstdIpc * self)
{
  GST_INFO_OBJECT (self, "Initializing gstd IPC");
  self->enabled = FALSE;
  self->session = NULL;
}

static void
gstd_ipc_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstdIpc *self = GSTD_IPC (object);

  gstd_object_set_code (GSTD_OBJECT (self), GSTD_EOK);

  switch (property_id) {

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      gstd_object_set_code (GSTD_OBJECT (self), GSTD_NO_RESOURCE);
      break;
  }
}

static void
gstd_ipc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdIpc *self = GSTD_IPC (object);

  gstd_object_set_code (GSTD_OBJECT (self), GSTD_EOK);

  switch (property_id) {
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      gstd_object_set_code (GSTD_OBJECT (self), GSTD_NO_RESOURCE);
      break;
  }
}

static void
gstd_ipc_dispose (GObject * object)
{
  GstdIpc *self = GSTD_IPC (object);

  GST_INFO_OBJECT (self, "Deinitializing gstd IPC");

  if (self->session)
    g_object_unref (self->session);

  self->session = NULL;

  G_OBJECT_CLASS (gstd_ipc_parent_class)->dispose (object);
}


gboolean
gstd_ipc_get_option_group (GstdIpc * ipc, GOptionGroup ** group)
{
  GstdIpcClass *klass;
  g_return_if_fail (ipc);
  klass = GSTD_IPC_GET_CLASS (ipc);
  return klass->get_option_group (ipc, group);
}

GstdReturnCode
gstd_ipc_start (GstdIpc * ipc, GstdSession * session)
{
  GstdIpcClass *klass;

  g_return_if_fail (ipc);
  g_return_if_fail (session);


  ipc->session = g_object_ref (session);

  klass = GSTD_IPC_GET_CLASS (ipc);
  klass->start (ipc, session);
}

GstdReturnCode
gstd_ipc_stop (GstdIpc * ipc)
{
  GstdIpcClass *klass;
  g_return_if_fail (ipc);
  klass = GSTD_IPC_GET_CLASS (ipc);
  return klass->stop (ipc);
}
