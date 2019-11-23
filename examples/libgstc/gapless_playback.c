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

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libgstc.h"

#define PIPELINE_DESC_FORMAT "playbin uri=file://%s"

static int running = 1;

static void
sig_handler (int sig)
{
  printf ("Closing...\n");
  running = 0;
}

int
main (int argc, char *argv[])
{
  GstClient *client;
  GstcStatus ret = 0;
  const char *address = "127.0.0.1";
  const unsigned int port = 5000;
  const long wait_time = -1;
  const int keep_open = 1;
  char *message;

  /* Seek parameters */
  const double rate = 1.0;
  const int format = 3;
  const int flags = 1;
  const int start_type = 1;
  const long start = 0;
  const int end_type = 1;
  const long end = -1;
  int asprintf_ret;
  const char *pipe_name = "pipe";
  char *pipe_description;
  char *video_name;

  if (argc != 2) {
    fprintf (stderr, "Please provide a video to play\n");
    goto out;
  }

  ret = gstc_client_new (address, port, wait_time, keep_open, &client);
  if (GSTC_OK != ret) {
    fprintf (stderr, "There was a problem creating a GstClient: %d\n", ret);
    goto out;
  }

  video_name = argv[1];
  asprintf_ret = asprintf (&pipe_description, PIPELINE_DESC_FORMAT, video_name);
  if (-1 == asprintf_ret) {
    goto free_client;
  }

  ret = gstc_pipeline_create (client, pipe_name, pipe_description);
  if (GSTC_OK == ret) {
    printf ("Pipeline created successfully!\n");
  } else {
    fprintf (stderr, "Error creating pipeline: %d\n", ret);
    goto free_resources;
  }

  ret = gstc_pipeline_play (client, pipe_name);
  if (GSTC_OK == ret) {
    printf ("Pipeline set to playing!\n");
  } else {
    fprintf (stderr, "Unable to play pipeline: %d\n", ret);
    goto pipeline_delete;
  }

  printf ("Press ctrl+c to stop the pipeline...\n");
  signal (SIGINT, sig_handler);

  while (running) {
    ret = gstc_pipeline_bus_wait (client, pipe_name, "eos", -1, &message);
    if (GSTC_OK == ret) {
      printf ("EOS message recieved!\n");
    } else {
      fprintf (stderr, "Unable to read from bus: %d\n", ret);
      goto close_pipeline;
    }

    ret =
        gstc_pipeline_seek (client, pipe_name, rate, format, flags, start_type,
        start, end_type, end);
    if (GSTC_OK == ret) {
      printf ("Pipeline reset!\n");
    } else {
      fprintf (stderr, "Unable to reset pipeline: %d\n", ret);
      goto close_pipeline;
    }
  }

close_pipeline:

  ret = gstc_pipeline_stop (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("Pipeline set to null!\n");
  } else {
    fprintf (stderr, "Unable to stop pipeline: %d\n", ret);
    goto free_client;
  }

pipeline_delete:
  ret = gstc_pipeline_delete (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("Pipeline deleted!\n");
  } else {
    fprintf (stderr, "Unable to delete pipeline: %d\n", ret);
    goto free_client;
  }

free_resources:
  free(pipe_description);
free_client:
  gstc_client_free (client);

out:
  return ret;
}
