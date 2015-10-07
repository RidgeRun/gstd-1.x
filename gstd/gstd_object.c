/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
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
#include <stdarg.h>
#include <gobject/gvaluecollector.h>
#include "gstd_object.h"
#include "gstd_list.h"

enum {
  PROP_NAME = 1,
  N_PROPERTIES // NOT A PROPERTY
};

/* Gstd Core debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_object_debug);
#define GST_CAT_DEFAULT gstd_object_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_DEFINE_TYPE (GstdObject, gstd_object, G_TYPE_OBJECT)

/* VTable */
static void
gstd_object_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_object_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_object_dispose (GObject *);
static GstdReturnCode
gstd_object_create_default (GstdObject *, const gchar *, va_list);
static GstdReturnCode
gstd_object_read_default (GstdObject *, const gchar *, va_list);
static GstdReturnCode
gstd_object_update_default (GstdObject *, const gchar *, va_list);
static GstdReturnCode
gstd_object_delete_default (GstdObject *, const gchar *);
static GstdReturnCode
gstd_object_to_string_default (GstdObject *object, gchar **outstring);

GType
gstd_object_flags_get_type (void)
{
  static GType param_flags_type = 0;
  static const GFlagsValue flags_types[] = {
    {GSTD_PARAM_CREATE, "CREATE", "create"},
    {GSTD_PARAM_READ, "READ", "read"},
    {GSTD_PARAM_UPDATE, "UPDATE", "update"},
    {GSTD_PARAM_DELETE, "DELETE", "delete"},
    {0, NULL, NULL}
  };
  if (!param_flags_type) {
    param_flags_type =
        g_flags_register_static ("GstdParamFlags", flags_types);
  }
  return param_flags_type;
}

static void
gstd_object_class_init (GstdObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_object_set_property;
  object_class->get_property = gstd_object_get_property;
  object_class->dispose = gstd_object_dispose;

  properties[PROP_NAME] =
    g_param_spec_string ("name",
			 "Name",
			 "The name of the current Gstd session",
			 GSTD_OBJECT_DEFAULT_NAME,
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 G_PARAM_READWRITE |
			 GSTD_PARAM_READ
			 );
  
  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  klass->create = gstd_object_create_default;
  klass->read = gstd_object_read_default;
  klass->update = gstd_object_update_default;
  klass->delete = gstd_object_delete_default;
  klass->to_string = gstd_object_to_string_default;
  
  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_object_debug, "gstdobject", debug_color,
			   "Gstd Object category");
}

static void
gstd_object_init (GstdObject *self)
{
  GST_DEBUG_OBJECT(self, "Initializing gstd object");

  self->name = g_strdup(GSTD_OBJECT_DEFAULT_NAME);
  self->code = GSTD_EOK;
  
  g_mutex_init (&self->codelock);
}

static void
gstd_object_get_property (GObject        *object,
		   guint           property_id,
		   GValue         *value,
		   GParamSpec     *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);

  switch (property_id) {
  case PROP_NAME:
    GST_DEBUG_OBJECT(self, "Returning object name \"%s\"", self->name);
    g_value_set_string (value, self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    return;
  }

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
}

static void
gstd_object_set_property (GObject      *object,
		   guint         property_id,
		   const GValue *value,
		   GParamSpec   *pspec)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  switch (property_id) {
  case PROP_NAME:
    if (self->name)
      g_free(self->name);
    
    self->name = g_value_dup_string (value);
    GST_INFO_OBJECT(self, "Changed object name to %s", self->name);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    return;
  }

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);
}

static void
gstd_object_dispose (GObject *object)
{
  GstdObject *self = GSTD_OBJECT(object);
  
  GST_DEBUG_OBJECT(object, "Deinitializing %s object", GSTD_OBJECT_NAME(self));

  if (self->name) {
    g_free (self->name);
    self->name = NULL;
  }
  
  G_OBJECT_CLASS(gstd_object_parent_class)->dispose(object);
}

static GstdReturnCode
gstd_object_create_default (GstdObject *object, const gchar *property,
			    va_list va)
{
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  GST_ERROR_OBJECT(object, "Cannot create resources in %s", GSTD_OBJECT_NAME(object));
  g_return_val_if_reached (GSTD_NO_CREATE);
}

static GstdReturnCode
gstd_object_read_default (GstdObject *self, const gchar *property,
			   va_list va)
{
  GParamSpec *pspec;
  const gchar *name;
  GstdReturnCode ret;
  GValue value = G_VALUE_INIT;
  gchar *error = NULL;
  
  g_return_val_if_fail (GSTD_IS_OBJECT (self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  name = property;
  ret = GSTD_EOK;

  while (name) {
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(self),
					  name);
    if (!pspec) {
      GST_ERROR_OBJECT (self, "The property %s is not a property in %s",
			name, GSTD_OBJECT_NAME(self));
      ret |= GSTD_NO_CREATE;
      break;
    } 

    if (!GSTD_PARAM_IS_READ(pspec->flags)) {
      GST_ERROR_OBJECT (self, "The property %s is not readable", name);
      ret |= GSTD_NO_READ;
      break;
    }
    
    g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
    g_object_get_property (G_OBJECT(self), name, &value);
    
    G_VALUE_LCOPY(&value, va, 0, &error);
    
    if (error) {
      GST_ERROR_OBJECT(self, "%s", error);
      g_free (error);
      ret |= GSTD_NO_CREATE;
    } else {
      GST_INFO_OBJECT(self, "Read object %s from %s", property,
		      GSTD_OBJECT_NAME(self));
    }

    g_value_unset (&value);
    name = va_arg (va, const gchar *);  
  }
  
  return ret;
}

static GstdReturnCode
gstd_object_update_default (GstdObject *self, const gchar *property,
			    va_list va)
{
  GParamSpec *pspec;
  const gchar *name;
  GstdReturnCode ret;
  GValue value = G_VALUE_INIT;
  gchar *error = NULL;
  
  g_return_val_if_fail (GSTD_IS_OBJECT (self), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  name = property;
  ret = GSTD_EOK;

  while (name) {
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(self), name);
    if (!pspec) {
      GST_ERROR_OBJECT (self, "The property %s is not a property in %s",
			name, GSTD_OBJECT_NAME(self));
      ret |= GSTD_NO_UPDATE;
      break;
    } 

    if (pspec->flags & G_PARAM_WRITABLE & !G_PARAM_CONSTRUCT_ONLY) {
      GST_ERROR_OBJECT (self, "The property %s is not writable", name);
      ret |= GSTD_NO_UPDATE;
      break;
    }
    
    g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
    G_VALUE_COLLECT(&value, va, 0, &error);
    if (error) {
      GST_ERROR_OBJECT(self, "%s", error);
      g_free (error);
      ret |= GSTD_NO_CREATE;
    } else {
      g_object_set_property (G_OBJECT(self), name, &value);
      GST_INFO_OBJECT(self, "Wrote object %s from %s", property,
		      GSTD_OBJECT_NAME(self));
    }

    g_value_unset (&value);
    name = va_arg (va, const gchar *);  
  }
  
  return ret;
}


static GstdReturnCode
gstd_object_delete_default (GstdObject *object, const gchar *name)
{
  return GSTD_EOK;
}


static GstdReturnCode
gstd_object_to_string_default (GstdObject *self, gchar **outstring)
{
  GParamSpec **properties;
  GValue value = G_VALUE_INIT;
  gchar *svalue;
  GValue flags = G_VALUE_INIT;
  gchar *sflags;
  guint n, i;
  const gchar *typename;
  const gchar *prolog = "{\n  [{\n";
  const gchar *epilog = "}]\n}";
  const gchar *new = "},{\n";
  const gchar *fmt = "%s    name       : %s,\n"
    "    value      : %s,\n"
    "    param_spec : {\n"
    "      blurb     : %s,\n"
    "      type      : %s,\n"
    "      access    : %s,\n"
    "      construct : %s,\n"
#if 0
    "      create    : %s,\n"
    "      read      : %s,\n"
    "      update    : %s,\n"
    "      delete    : %s\n"
#endif
    "    }\n  %s";
  
  gchar *buffer;
  gchar *oldbuffer;
  
  g_return_val_if_fail (GSTD_IS_OBJECT(self), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  buffer = g_strdup(prolog);
  
  properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(self), &n);
  for (i=0;i<n;i++) {
    typename = g_type_name(properties[i]->value_type);

    g_value_init (&value, properties[i]->value_type);
    g_object_get_property(G_OBJECT(self), properties[i]->name, &value);
    svalue = g_strdup_value_contents(&value);
    g_value_unset(&value);

    g_value_init (&flags, GSTD_TYPE_PARAM_FLAGS);
    g_value_set_flags (&flags, properties[i]->flags);
    sflags = g_strdup_value_contents(&flags);
    g_value_unset(&flags);
      
    oldbuffer = buffer;
    buffer = g_strdup_printf(fmt, oldbuffer,
        properties[i]->name, svalue,
        properties[i]->_blurb,
        typename,
        sflags,
	G_PARAM_CONSTRUCT_ONLY & properties[i]->flags ? "TRUE" : "FALSE",
#if 0
	GSTD_PARAM_IS_CREATE(properties[i]->flags) ? "TRUE" : "FALSE",
	GSTD_PARAM_IS_READ(properties[i]->flags) ? "TRUE" : "FALSE",
	GSTD_PARAM_IS_UPDATE(properties[i]->flags) ? "TRUE" : "FALSE",
        GSTD_PARAM_IS_DELETE(properties[i]->flags) ? "TRUE" : "FALSE",
#endif
        i+1 != n ? new : epilog);

    g_free (sflags);
    g_free (svalue);
    g_free (oldbuffer);
  }
  g_free (properties);
  *outstring = buffer;
  return GSTD_EOK;
}

void
gstd_object_set_code (GstdObject *self, GstdReturnCode code)
{
  GST_LOG_OBJECT (self, "Setting return code to %d", code);
  
  g_mutex_lock (&self->codelock);
  self->code = code;
  g_mutex_unlock (&self->codelock);
}

GstdReturnCode
gstd_object_get_code (GstdObject *self)
{
  GstdReturnCode code;

  g_mutex_lock (&self->codelock);
  code = self->code;
  g_mutex_unlock (&self->codelock);
  
  GST_LOG_OBJECT (self, "Returning code %d", code);
  return code;
}


GstdReturnCode
gstd_object_create (GstdObject *object, const gchar *property, ...)
{
  va_list va;
  GstdReturnCode ret;
  
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  va_start(va, property);
  ret = GSTD_OBJECT_GET_CLASS(object)->create (object, property, va);
  va_end(va);

  return ret;
}

GstdReturnCode
gstd_object_read (GstdObject *object, const gchar *property, ...)
{
  va_list va;
  GstdReturnCode ret;
  
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  va_start(va, property);
  ret = GSTD_OBJECT_GET_CLASS(object)->read (object, property, va);
  va_end(va);

  return ret;
}

GstdReturnCode
gstd_object_update (GstdObject *object, const gchar *property, ...)
{
  va_list va;
  GstdReturnCode ret;
  
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (property, GSTD_NULL_ARGUMENT);

  va_start(va, property);
  ret = GSTD_OBJECT_GET_CLASS(object)->update (object, property, va);
  va_end(va);

  return ret;
}

GstdReturnCode
gstd_object_delete (GstdObject *object, const gchar *name)
{
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (name, GSTD_NULL_ARGUMENT);

  return GSTD_OBJECT_GET_CLASS(object)->delete (object, name);
}

GstdReturnCode
gstd_object_to_string (GstdObject *object, gchar **outstring)
{
  g_return_val_if_fail (GSTD_IS_OBJECT(object), GSTD_NULL_ARGUMENT);
  g_warn_if_fail (!*outstring);

  return GSTD_OBJECT_GET_CLASS(object)->to_string (object, outstring);
}
