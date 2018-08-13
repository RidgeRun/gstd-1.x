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
#include <poll.h>
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

static int create_new_socket ();
static GstcStatus open_socket(GstcSocket *self);

struct _GstcSocket
{
  int socket;
  struct sockaddr_in server;
  int keep_connection_open;
};

static int
create_new_socket ()
{
  const int domain = AF_INET;
  const int type = SOCK_STREAM;
  const int proto = 0;

  return socket (domain, type, proto);
}

static GstcStatus
open_socket(GstcSocket *self)
{
  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);

  self->socket = create_new_socket ();
  if (-1 == self->socket) {
    return GSTC_SOCKET_ERROR;
  }

  if (connect (self->socket, (struct sockaddr *) &self->server,
          sizeof (self->server)) < 0) {
    close (self->socket);
    return GSTC_UNREACHABLE;
  }
  return GSTC_OK;
}

GstcStatus
gstc_socket_new (const char *address, const unsigned int port,
    const int keep_connection_open, GstcSocket ** out)
{
  GstcStatus ret;
  GstcSocket *self;
  const int domain = AF_INET;

  gstc_assert_and_ret_val (NULL != address, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != out, GSTC_NULL_ARGUMENT);

  *out = NULL;

  self = (GstcSocket *) malloc (sizeof (GstcSocket));
  if (NULL == self) {
    ret = GSTC_OOM;
    goto out;
  }

  self->keep_connection_open = keep_connection_open;
  
  self->server.sin_addr.s_addr = inet_addr (address);
  self->server.sin_family = domain;
  self->server.sin_port = htons (port);

  if (self->keep_connection_open) {
    ret = open_socket(self);
    if (ret != GSTC_OK) {
      goto free_self;
    }
  }

  ret = GSTC_OK;
  *out = self;
  goto out;

free_self:
  free (self);
  self = NULL;

out:
  return ret;
}

GstcStatus
gstc_socket_send (GstcSocket * self, const char *request, char **response,
    const int timeout)
{
  int rv;
  const int number_of_sockets = 1;
  struct pollfd ufds[number_of_sockets];
  GstcStatus ret;

  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != request, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != response, GSTC_NULL_ARGUMENT);

  if (!self->keep_connection_open) {
    ret = open_socket(self);
    if (ret != GSTC_OK) {
      goto out;
    }
  }

  if (send (self->socket, request, strlen (request), 0) < 0) {
    ret = GSTC_SEND_ERROR;
    goto out;
  }

  *response = malloc (GSTC_MAX_RESPONSE_LENGTH);

  ufds[0].fd = self->socket;
  ufds[0].events = POLLIN;
  
  rv = poll (ufds, number_of_sockets, timeout);

  /* Error ocurred in poll */
  if (rv == -1) {
    return GSTC_SOCKET_ERROR;
  }
  /* Timeout ocurred */
  else if (rv == 0) {
    return GSTC_SOCKET_TIMEOUT;
  } else {
    /* Check for events on the socket */
    if (ufds[0].revents & POLLIN) {
      if (recv (self->socket, *response, GSTC_MAX_RESPONSE_LENGTH, 0) < 0) {
        return GSTC_RECV_ERROR;
      }
    } else {
      return GSTC_SOCKET_ERROR;
    }
  }

  ret = GSTC_OK;

out:
  if (!self->keep_connection_open) {
    close (self->socket);
  }

  return ret;
}

void
gstc_socket_free (GstcSocket * socket)
{
  gstc_assert_and_ret (NULL != socket);

  if (socket->keep_connection_open) {
    close (socket->socket);
  }
  free (socket);
}
