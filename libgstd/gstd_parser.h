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

#ifndef __GSTD_PARSER_H__
#define __GSTD_PARSER_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

#include "gstd_return_codes.h"
#include "gstd_session.h"

/**
 * Parses a command received from the client.
 * 
 * \param session GstdSession object.
 * \param cmd Command line to be parsed
 * \param response Reference to the object where the result will be stored.
 * 
 * \return GstdReturnCode return code for the transaction.
 **/
GstdReturnCode gstd_parser_parse_cmd (GstdSession * session, const gchar * cmd,
    gchar ** response);

#endif // __GSTD_PARSER_H__
