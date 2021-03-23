/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2021 Ridgerun, LLC (http://www.ridgerun.com)
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

#include "gstd_refcount.h"

/* Hash table values struct */
typedef struct
{
  /**
   * Refcount for create calls
   */
  guint create_refcount;

  /**
   * Refcount for play calls
   */
  guint play_refcount;
} HashValue;

static void gstd_refcount_finalize (GObject * object);
static HashValue *gstd_refcount_get_hash_value (GstdRefcount * refcount,
    gchar * key);

struct _GstdRefcount
{
  GObject parent;

  /**
   * Hash table to keep track of the refcounts
   */
  GHashTable *hash;

  /**
   * Protection for object's lock
   */
  GMutex mutex;
};

G_DEFINE_TYPE (GstdRefcount, gstd_refcount, G_TYPE_OBJECT);

static void
gstd_refcount_class_init (GstdRefcountClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = gstd_refcount_finalize;
}

static void
gstd_refcount_init (GstdRefcount * refcount)
{
  refcount->hash = g_hash_table_new (g_str_hash, g_str_equal);

  g_mutex_init (&refcount->mutex);
}

static void
gstd_refcount_finalize (GObject * object)
{
  GstdRefcount *refcount;

  refcount = _GSTD_REFCOUNT (object);

  g_hash_table_destroy (refcount->hash);

  g_mutex_clear (&refcount->mutex);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (gstd_refcount_parent_class)->finalize (object);
}

static HashValue *
gstd_refcount_get_hash_value (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;

  if ((value = g_hash_table_lookup (refcount->hash, key)) == NULL) {
    /* key not found */
    value = (HashValue *) malloc (sizeof (HashValue));
    value->create_refcount = 0;
    value->play_refcount = 0;
    g_hash_table_insert (refcount->hash, key, value);
  }
  return value;
}

guint
gstd_refcount_get_create_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;
  guint create_refcount;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  create_refcount = value->create_refcount;
  g_mutex_unlock (&refcount->mutex);

  return create_refcount;
}

guint
gstd_refcount_get_play_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;
  guint play_refcount;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  play_refcount = value->play_refcount;
  g_mutex_unlock (&refcount->mutex);

  return play_refcount;
}

void
gstd_refcount_increment_create_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  value->create_refcount++;
  g_mutex_unlock (&refcount->mutex);
}

void
gstd_refcount_decrement_create_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  if (value->create_refcount > 0) {
    value->create_refcount--;
  }
  g_mutex_unlock (&refcount->mutex);
}

void
gstd_refcount_increment_play_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  value->play_refcount++;
  g_mutex_unlock (&refcount->mutex);
}

void
gstd_refcount_decrement_play_refcount (GstdRefcount * refcount, gchar * key)
{
  HashValue *value;

  g_mutex_lock (&refcount->mutex);
  value = gstd_refcount_get_hash_value (refcount, key);
  if (value->play_refcount > 0) {
    value->play_refcount--;
  }
  g_mutex_unlock (&refcount->mutex);
}
