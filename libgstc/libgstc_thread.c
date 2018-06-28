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

#include <pthread.h>
#include <stdlib.h>

#include "libgstc_assert.h"
#include "libgstc_thread.h"

struct _GstcThread
{
  pthread_t thread;
};

GstcStatus
gstc_thread_new (GstcThreadFunction func, void *user_data, GstcThread ** self)
{
  GstcThread *out;
  int tret;

  gstc_assert_and_ret_val (NULL != func, GSTC_NULL_ARGUMENT);
  gstc_assert_and_ret_val (NULL != self, GSTC_NULL_ARGUMENT);

  out = malloc (sizeof (GstcThread));

  tret = pthread_create (&(out->thread), NULL, func, user_data);
  if (tret) {
    free (out);
    return GSTC_THREAD_ERROR;
  }

  *self = out;

  return GSTC_OK;
}

void
gstc_thread_free (GstcThread * self)
{
  gstc_assert_and_ret (NULL != self);

  pthread_join (self->thread, NULL);

  free (self);
}
