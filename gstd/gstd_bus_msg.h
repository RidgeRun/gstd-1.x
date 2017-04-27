/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#ifndef __GSTD_BUS_MSG_H__
#define __GSTD_BUS_MSG_H__

#include <glib-object.h>

#include "gstd_object.h"

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define GSTD_TYPE_BUS_MSG \
  (gstd_bus_msg_get_type())
#define GSTD_BUS_MSG(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_BUS_MSG,GstdBusMsg))
#define GSTD_BUS_MSG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_BUS_MSG,GstdBusMsgClass))
#define GSTD_IS_BUS_MSG(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_BUS_MSG))
#define GSTD_IS_BUS_MSG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_BUS_MSG))
#define GSTD_BUS_MSG_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_BUS_MSG, GstdBusMsgClass))

typedef struct _GstdBusMsg GstdBusMsg;
typedef struct _GstdBusMsgClass GstdBusMsgClass;
GType gstd_bus_msg_get_type();

struct _GstdBusMsg
{
  GstdObject parent;

  GstMessage * target;
};

struct _GstdBusMsgClass
{
  GstdObjectClass parent_class;
};

GstdBusMsg *
gstd_bus_msg_factory_make (GstMessage * target);

G_END_DECLS

#endif // __GSTD_BUS_MSG_H__
