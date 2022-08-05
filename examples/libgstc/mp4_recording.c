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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libgstc.h"

int
main (int argc, char *argv[])
{
  GstClient *client;
  GstcStatus ret;
  const char *address = "127.0.0.1";
  const unsigned int port = 5000;
  const long wait_time = -1;
  const int keep_open = 1;
  char *eos;

  ret = gstc_client_new (address, port, wait_time, keep_open, &client);
  if (GSTC_OK != ret) {
    fprintf (stderr, "There was a problem creating a GstClient: %d\n", ret);
    goto out;
  }

  ret = gstc_pipeline_create (client, "pipe",
      "qtmux name=mux ! filesink location=mp4_recording.mp4 "
      "videotestsrc is-live=true ! avenc_mpeg4 ! mux. "
      "audiotestsrc is-live=true ! lamemp3enc ! mux. ");
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
    goto pipeline_delete;
  }

  printf ("Press any enter to stop pipeline...\n");
  getchar ();

  /* EOS is necessary to properly fill MP4 atoms, otherwise
     file will result unreadable */
  ret = gstc_pipeline_inject_eos (client, "pipe");
  if (GSTC_OK == ret) {
    printf ("EOS sent!\n");
  } else {
    fprintf (stderr, "Unable to inject EOS to the pipeline: %d\n", ret);
    goto close_pipeline;
  }

  printf ("Waiting for EOS... ");
  ret = gstc_pipeline_bus_wait (client, "pipe", "eos", 10000000000, &eos);
  if (GSTC_OK == ret) {
    printf ("received!\n");
    free (eos);
  } else if (GSTC_BUS_TIMEOUT == ret) {
    printf ("timeout!\n");
    fprintf (stderr, "EOS not received, file will be unreadable\n");
  } else {
    printf ("error!\n");
    fprintf (stderr, "An error occurred waiting for EOS: %d\n", ret);
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
    fprintf (stderr, "Unable to delete ipeline: %d\n", ret);
    goto free_client;
  }

free_client:
  gstc_client_free (client);

out:
  return ret;
}
