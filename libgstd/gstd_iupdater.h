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

#ifndef __GSTD_IUPDATER_H__
#define __GSTD_IUPDATER_H__

#include <gst/gst.h>
#include "gstd_return_codes.h"

G_BEGIN_DECLS
#define GSTD_TYPE_IUPDATER                (gstd_iupdater_get_type ())
#define GSTD_IUPDATER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_IUPDATER, GstdIUpdater))
#define GSTD_IS_IUPDATER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_IUPDATER))
#define GSTD_IUPDATER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_IUPDATER, GstdIUpdaterInterface))
typedef struct _GstdIUpdater GstdIUpdater;
typedef struct _GstdIUpdaterInterface GstdIUpdaterInterface;

// Avoid cyclic dependecies by forward declaration
typedef struct _GstdObject GstdObject;

struct _GstdIUpdaterInterface
{
  GTypeInterface parent;

  GstdReturnCode (*update) (GstdIUpdater * self, GstdObject * object,
      const gchar * value);
};

GType gstd_iupdater_get_type (void);

GstdReturnCode gstd_iupdater_update (GstdIUpdater * self, GstdObject * object,
    const gchar * value);

G_END_DECLS
#endif // __GSTD_IUPDATER_H__
