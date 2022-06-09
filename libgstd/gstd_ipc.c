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
  N_PROPERTIES,                 // NOT A PROPERTY
  PROP_ENABLED
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

  /* Properties */
  g_object_class_install_property (object_class, PROP_ENABLED,
      g_param_spec_boolean ("enabled", "ENABLED",
          "IPC enable", FALSE, G_PARAM_READWRITE));

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

  switch (property_id) {
    case PROP_ENABLED:
      g_value_set_boolean (value, self->enabled);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gstd_ipc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstdIpc *self = GSTD_IPC (object);

  switch (property_id) {
    case PROP_ENABLED:
      self->enabled = g_value_get_boolean (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
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
  g_return_val_if_fail (ipc, FALSE);
  klass = GSTD_IPC_GET_CLASS (ipc);
  return klass->get_option_group (ipc, group);
}

GstdReturnCode
gstd_ipc_start (GstdIpc * ipc, GstdSession * session)
{
  GstdIpcClass *klass;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (ipc, GSTD_IPC_ERROR);
  g_return_val_if_fail (session, GSTD_IPC_ERROR);

  ipc->session = g_object_ref (session);

  if (TRUE == ipc->enabled) {
    klass = GSTD_IPC_GET_CLASS (ipc);
    ret = klass->start (ipc, session);
  }

  return ret;
}

GstdReturnCode
gstd_ipc_stop (GstdIpc * ipc)
{
  GstdIpcClass *klass;
  GstdReturnCode ret = GSTD_EOK;

  g_return_val_if_fail (GSTD_IS_OBJECT (ipc), GSTD_IPC_ERROR);

  if (TRUE == ipc->enabled) {
    klass = GSTD_IPC_GET_CLASS (ipc);
    ret = klass->stop (ipc);
  }

  if (ipc->session) {
    g_object_unref (ipc->session);
    ipc->session = NULL;
  }

  return ret;
}
