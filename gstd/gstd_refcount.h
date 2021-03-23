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

#ifndef __GSTD_REFCOUNT_H__
#define __GSTD_REFCOUNT_H__

#include <glib-object.h>

G_BEGIN_DECLS
#define TYPE_GSTD_REFCOUNT (gstd_refcount_get_type ())
G_DECLARE_FINAL_TYPE (GstdRefcount, gstd_refcount,, GSTD_REFCOUNT, GObject);

typedef struct _GstdRefcount GstdRefcount;

/**
 * Get the create refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return guint refcount
 **/
guint gstd_refcount_get_create_refcount (GstdRefcount * refcount, gchar * key);

/**
 * Get the play refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return guint refcount
 **/
guint gstd_refcount_get_play_refcount (GstdRefcount * refcount, gchar * key);

/**
 * Increment the create refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return void
 **/
void gstd_refcount_increment_create_refcount (GstdRefcount * refcount,
    gchar * key);

/**
 * Decrement the create refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return void
 **/
void gstd_refcount_decrement_create_refcount (GstdRefcount * refcount,
    gchar * key);

/**
 * Increment the play refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return void
 **/
void gstd_refcount_increment_play_refcount (GstdRefcount * refcount,
    gchar * key);

/**
 * Decrement the play refcount of a given key
 *
 * \param refcount GstdRefcount object
 * \param key Pipeline name
 *
 * \return void
 **/
void gstd_refcount_decrement_play_refcount (GstdRefcount * refcount,
    gchar * key);

G_END_DECLS
#endif //__GSTD_REFCOUNT_H__
