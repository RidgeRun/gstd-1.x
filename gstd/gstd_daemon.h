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
#ifndef __GSTD_DAEMON_H__
#define __GSTD_DAEMON_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

/**
 * Initializes the resources necessary to daemonize or kill a 
 * a process.
 *
 * \param argc The amount of parameters given in argv 
 * \param argv The array of cmdline arguments given to 
 * the process in the application's main
 *
 * \return TRUE if the resources were initilized succesfully, FALSE
 * otherwise.
 */
gboolean gstd_daemon_init (gint argc, gchar * argv[], gchar * pidfilename);

/**
 * Daemonizes the current process using the given process name
 *
 * \param parent A memory location to hold either the fork belongs to
 * the parent of the child.
 *
 * \return TRUE if the process was sucessfully daemonized, FALSE
 * otherwise.
 */
gboolean gstd_daemon_start (gboolean * parent);

/**
 * Closes the resources associated with the daemon, if any.
 *
 * \return TRUE if the daemon was successfully closed, FALSE
 * otherwise.
 */
gboolean gstd_daemon_stop (void);

#endif // __GSTD_DAEMON_H__
