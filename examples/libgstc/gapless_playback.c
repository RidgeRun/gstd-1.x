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
  free (pipe_description);
free_client:
  gstc_client_free (client);

out:
  return ret;
}
