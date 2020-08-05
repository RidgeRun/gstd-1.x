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
#  define GSTC_MAX_RESPONSE_LENGTH 10485760 //10 * 1024 * 1024
#endif

#define NUMBER_OF_SOCKETS (1)

static int create_new_socket ();
static GstcStatus open_socket (GstcSocket * self);
static GstcStatus accumulate_response (int socket, char **response);

struct _GstcSocket
{
  int socket;
  struct sockaddr_in server;
  int keep_connection_open;
};

static int
create_new_socket (void)
{
  const int domain = AF_INET;
  const int type = SOCK_STREAM;
  const int proto = 0;

  return socket (domain, type, proto);
}

static GstcStatus
open_socket (GstcSocket * self)
{
  int buffsize = GSTC_MAX_RESPONSE_LENGTH;
  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);

  self->socket = create_new_socket ();
  if (-1 == self->socket) {
    return GSTC_SOCKET_ERROR;
  }

  if (setsockopt (self->socket, SOL_SOCKET, SO_RCVBUF, &buffsize,
          sizeof (buffsize))) {
    close (self->socket);
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
    ret = open_socket (self);
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

static GstcStatus
accumulate_response (int socket, char **response)
{
  char buffer[1024];
  ssize_t read = 0;
  size_t acc = 0;
  int flags = 0;
  const char terminator = '\0';
  GstcStatus ret = GSTC_OK;
  char *dst = NULL;

  gstc_assert_and_ret_val (NULL != response, GSTC_NULL_ARGUMENT);

  *response = NULL;

  do {
    read = recv (socket, buffer, sizeof(buffer), flags);

    if (read < 0) {
      ret = GSTC_RECV_ERROR;
      break;
    }

    *response = realloc (*response, acc + read);

    dst = *response + acc;
    acc += read;

    if (acc >= GSTC_MAX_RESPONSE_LENGTH) {
      ret = GSTC_LONG_RESPONSE;
      free (*response);
      *response = NULL;
      break;
    }

    memcpy (dst, buffer, read);
  } while (buffer[read - 1] != terminator);

  return ret;
}

GstcStatus
gstc_socket_send (GstcSocket * self, const char *request, char **response,
    const int timeout)
{
  int rv;
  struct pollfd ufds[NUMBER_OF_SOCKETS];
  GstcStatus ret;

  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != request, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != response, GSTC_NULL_ARGUMENT);

  if (!self->keep_connection_open) {
    ret = open_socket (self);
    if (ret != GSTC_OK) {
      goto out;
    }
  }

  if (send (self->socket, request, strlen (request), 0) < 0) {
    ret = GSTC_SEND_ERROR;
    goto close_con;
  }

  ufds[0].fd = self->socket;
  ufds[0].events = POLLIN;

  rv = poll (ufds, NUMBER_OF_SOCKETS, timeout);

  /* Error ocurred in poll */
  if (rv == -1) {
    ret = GSTC_SOCKET_ERROR;
    goto close_con;
  }

  /* Timeout ocurred */
  if (rv == 0) {
    ret = GSTC_SOCKET_TIMEOUT;
    goto close_con;
  }

  /* Check for events on the socket */
  if (0 == (ufds[0].revents & POLLIN)) {
    ret = GSTC_SOCKET_ERROR;
    goto close_con;
  }

  ret = accumulate_response (self->socket, response);

close_con:
  if (!self->keep_connection_open) {
    close (self->socket);
  }

out:
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
