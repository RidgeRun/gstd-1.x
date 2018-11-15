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
#include <unistd.h>

#include "libgstc.h"

int
main ()
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
