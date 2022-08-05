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

static int quit = 0;

static void
sig_handler (int sig)
{
  printf ("Closing...\n");
  quit = 1;
}

int
main (int argc, char *argv[])
{
  GstClient *client;
  GstcStatus ret;
  const char *address = "127.0.0.1";
  const unsigned int port = 5000;
  const long wait_time = -1;
  const int keep_open = 1;
  int format = 0;

  ret = gstc_client_new (address, port, wait_time, keep_open, &client);
  if (GSTC_OK != ret) {
    fprintf (stderr, "There was a problem creating a GstClient: %d\n", ret);
    goto out;
  }

  ret =
      gstc_pipeline_create (client, "pipe",
      "videotestsrc name=vts ! autovideosink");
  if (GSTC_OK == ret) {
    printf ("Pipeline created successfully!\n");
  } else {
    fprintf (stderr, "Error creating pipeline: %d\n", ret);
    goto free_client;
  }

  ret = gstc_pipeline_play (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("Pipeline set to playing!\n");
  } else {
    fprintf (stderr, "Unable to play pipeline: %d\n", ret);
    goto free_client;
  }

  printf ("Press ctrl+c to stop the pipeline...\n");
  signal (SIGINT, sig_handler);

  while (0 == quit) {
    gstc_element_set (client, "pipe", "vts", "pattern", "%d", format);

    /* Cycle through the first videotestsrc patterns */
    format = (format + 1) % 10;
    sleep (1);
  }

  ret = gstc_pipeline_stop (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("Pipeline set to null!\n");
  } else {
    fprintf (stderr, "Unable to stop pipeline: %d\n", ret);
    goto free_client;
  }

  ret = gstc_pipeline_delete (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("Pipeline deleted!\n");
  } else {
    fprintf (stderr, "Unable to delete ipeline: %d\n", ret);
    goto free_client;
  }

free_client:
  gstc_client_free (client);

out:
  return ret;
}
