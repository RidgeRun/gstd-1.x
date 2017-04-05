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

#ifndef __GSTD_PROPERTY_INT_H__
#define __GSTD_PROPERTY_INT_H__

#include <glib-object.h>

#include "gstd_property.h"

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define GSTD_TYPE_PROPERTY_INT \
  (gstd_property_int_get_type())
#define GSTD_PROPERTY_INT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_PROPERTY_INT,GstdPropertyInt))
#define GSTD_PROPERTY_INT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_PROPERTY_INT,GstdPropertyIntClass))
#define GSTD_IS_PROPERTY_INT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_PROPERTY_INT))
#define GSTD_IS_PROPERTY_INT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_PROPERTY_INT))
#define GSTD_PROPERTY_INT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_PROPERTY_INT, GstdPropertyIntClass))

typedef struct _GstdPropertyInt GstdPropertyInt;
typedef struct _GstdPropertyIntClass GstdPropertyIntClass;
GType gstd_property_int_get_type();


struct _GstdPropertyInt
{
  GstdProperty parent;
};

struct _GstdPropertyIntClass
{
    GstdPropertyClass parent_class;
};

GstdReturnCode gstd_property_int_to_string (GstdObject * self, gchar ** outstring);

G_END_DECLS

#endif // __GSTD_PROPERTY_INT_H__
