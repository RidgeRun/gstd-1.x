/*
 * GStreamer Daemon - gst-launch on steroids
 * C client library abstracting gstd interprocess communication
 *
 * Copyright (c) 2015-2018 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "libgstc_assert.h"
#include "libgstc_socket.h"

/* Allow the user to override this value at build time */
#ifndef GSTC_MAX_RESPONSE_LENGTH
#  define GSTC_MAX_RESPONSE_LENGTH 4096
#endif

struct _GstcSocket
{
  int socket;
  struct sockaddr_in server;
};

GstcStatus
gstc_socket_new (const char *address, const unsigned int port,
    const unsigned long wait_time, const int keep_connection_open,
    GstcSocket ** out)
{
  GstcStatus ret;
  GstcSocket *self;
  const int domain = AF_INET;
  const int type = SOCK_STREAM;
  const int proto = 0;

  gstc_assert_and_ret_val (NULL != address, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  *out = NULL;

  self = (GstcSocket *) malloc (sizeof (GstcSocket));
  if (NULL == self) {
    ret = GSTC_OOM;
    goto out;
  }

  self->socket = socket (domain, type, proto);
  if (-1 == self->socket) {
    ret = GSTC_SOCKET_ERROR;
    goto free_self;
  }

  self->server.sin_addr.s_addr = inet_addr (address);
  self->server.sin_family = domain;
  self->server.sin_port = htons (port);

  if (connect (self->socket, (struct sockaddr *) &self->server,
          sizeof (self->server)) < 0) {
    ret = GSTC_UNREACHABLE;
    goto close_socket;
  }

  ret = GSTC_OK;
  *out = self;
  goto out;

close_socket:
  close (self->socket);

free_self:
  free (self);
  self = NULL;

out:
  return ret;
}

GstcStatus
gstc_socket_send (GstcSocket * self, const char *request, char **response)
{
  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != request, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != response, GSTC_NULL_ARGUMENT);

  if (send (self->socket, request, strlen (request), 0) < 0) {
    return GSTC_SEND_ERROR;
  }

  *response = malloc (GSTC_MAX_RESPONSE_LENGTH);

  if (recv (self->socket, *response, GSTC_MAX_RESPONSE_LENGTH, 0) < 0) {
    return GSTC_RECV_ERROR;
  }

  return GSTC_OK;
}

void
gstc_socket_free (GstcSocket * socket)
{
  close (socket->socket);
  free (socket);
}
