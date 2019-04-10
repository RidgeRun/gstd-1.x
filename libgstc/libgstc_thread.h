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

#ifndef __LIBGSTC_THREAD_H__
#define __LIBGSTC_THREAD_H__

#include <pthread.h>

#include "libgstc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _GstcThread GstcThread;
typedef struct _GstcCond GstcCond;
typedef struct _GstcMutex GstcMutex;

struct _GstcThread
{
  pthread_t thread;
};

struct _GstcCond
{
  pthread_cond_t cond;
};

struct _GstcMutex
{
  pthread_mutex_t mutex;
};

typedef void *(*GstcThreadFunction) (void *);
  
GstcStatus
gstc_thread_new (GstcThread *thread, GstcThreadFunction func, void * user_data);

void
gstc_mutex_init (GstcMutex *mutex);

void
gstc_mutex_lock (GstcMutex *mutex);

void
gstc_mutex_unlock (GstcMutex *mutex);

void
gstc_cond_init (GstcCond *mutex);

void
gstc_cond_wait (GstcCond *cond, GstcMutex *mutex);

void
gstc_cond_signal (GstcCond *cond);

#ifdef __cplusplus
}
#endif

#endif // __LIBGSTC_THREAD_H__
