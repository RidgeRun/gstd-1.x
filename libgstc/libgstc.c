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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgstc.h"
#include "libgstc_socket.h"
#include "libgstc_assert.h"

static GstcStatus gstc_cmd_send (GstClient * client, const char *request);
static GstcStatus gstc_cmd_create (GstClient * client, const char *where,
    const char *what);

struct _GstClient
{
  GstcSocket *socket;
};

static GstcStatus
gstc_cmd_send (GstClient * client, const char *request)
{
  libgstc_assert_and_ret_val (NULL != client, GSTC_NULL_ARGUMENT);
  libgstc_assert_and_ret_val (NULL != request, GSTC_NULL_ARGUMENT);

  return gstc_socket_send (client->socket, request);
}

static GstcStatus
gstc_cmd_create (GstClient * client, const char *where, const char *what)
{
  GstcStatus ret;
  const char *template = "create %s %s";
  char *request;

  libgstc_assert_and_ret_val (NULL != client, GSTC_NULL_ARGUMENT);
  libgstc_assert_and_ret_val (NULL != where, GSTC_NULL_ARGUMENT);
  libgstc_assert_and_ret_val (NULL != what, GSTC_NULL_ARGUMENT);

  /* Concatenate pieces into request */
  asprintf (&request, template, where, what);

  ret = gstc_cmd_send (client, request);

  free (request);

  return ret;
}

GstClient *
gstc_client_new (const char *address, const unsigned int port,
    const unsigned long wait_time, const int keep_connection_open)
{
  GstClient *client;

  libgstc_assert_and_ret_val (NULL != address, NULL);

  client = (GstClient *) malloc (sizeof (GstClient));
  if (NULL == client) {
    return NULL;
  }

  client->socket = gstc_socket_new (address, port, wait_time,
      keep_connection_open);

  return client;
}

GstcStatus
gstc_pipeline_create (GstClient * client, const char *pipeline_name,
    const char *pipeline_desc)
{
  GstcStatus ret;
  const char *resource = "/pipelines";
  const char *template = "%s %s";
  char *create_args;

  libgstc_assert_and_ret_val (NULL != client, GSTC_NULL_ARGUMENT);
  libgstc_assert_and_ret_val (NULL != pipeline_name, GSTC_NULL_ARGUMENT);
  libgstc_assert_and_ret_val (NULL != pipeline_desc, GSTC_NULL_ARGUMENT);

  asprintf (&create_args, template, pipeline_name, pipeline_desc);

  ret = gstc_cmd_create (client, resource, create_args);

  free (create_args);

  return ret;
}

void
gstc_client_free (GstClient * client)
{
  libgstc_assert_and_ret (NULL != client);

  gstc_socket_free (client->socket);
  free (client);
}
