/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "gstd_session.h"
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS (3)

static void
singleton_instantiation_test (void)
{
  GstdSession *temp1 = NULL, *temp2 = NULL;
  gchar *name1, *name2;
  GPid pid1 = 1, pid2 = 2;
  temp1 = gstd_session_new (NULL);
  g_assert_true (temp1 != temp2);
  temp2 = gstd_session_new ("Session0");
  g_assert_true (temp1 == temp2);
  g_print ("GstdSession temp1 ptr: %p, GstdSession temp2 ptr: %p\n", temp1,
      temp2);
  g_object_get (temp1, "pid", &pid1, NULL);
  g_object_get (temp2, "pid", &pid2, NULL);
  g_print ("PID 1: %d PID 2: %d  \n", pid1, pid2);
  g_object_get (temp1, "name", &name1, NULL);
  g_object_get (temp2, "name", &name2, NULL);
  g_print ("Name 1: %s Name 2: %s  \n", name1, name2);
  g_assert_cmpint (pid1, ==, pid1);
  g_object_unref (temp1);
  g_object_unref (temp2);
  g_free (name1);
  g_free (name2);
}

static void
session_mem_leak_test (void)
{
  gint reps = 10;
  gint i;
  for (i = 0; i < reps; ++i) {
    singleton_instantiation_test ();
  }
}



static void *
instantiate_session_singleton (gpointer address)
{
  GstdSession **sessionAdress = (GstdSession **) address;
  g_print ("Array Adress: %p, ", address);
  *sessionAdress = gstd_session_new ("SessionTest");
  g_print ("GstdSession ptr: %p \n", *sessionAdress);
  return NULL;
}

static void
thread_safety_instantiation_test (void)
{

  gint reps = 10;
  gint i, j, r;
  GThread *threads[NUM_THREADS];
  GstdSession *sessions[NUM_THREADS];

  //We need at least 2 threads to test
  g_assert_true (NUM_THREADS > 2);

  for (r = 0; r < reps; ++r) {
    //spawn threads
    for (i = 0; i < NUM_THREADS; ++i) {
      threads[i] =
          g_thread_new ("thread", instantiate_session_singleton,
          (gpointer) & sessions[i]);
    }

    //wait for threads to finish
    g_thread_join (threads[0]);
    for (j = 1; j < NUM_THREADS; ++j) {
      g_thread_join (threads[j]);
      //Check all singletons are the same
      g_print ("GstdSession ptr %d: %p, GstdSession ptr %d: %p \n", j - 1,
          sessions[j - 1], j, sessions[j]);
      g_assert_true (sessions[j - 1] == sessions[j]);
      g_object_unref (sessions[j - 1]);
    }
    g_object_unref (sessions[NUM_THREADS - 1]);
  }
}

gint
main (gint argc, gchar * argv[])
{
  /* Initialize GLib, deprecated in 2.36 */
#if !GLIB_CHECK_VERSION(2,36,0)
  g_type_init ();
#endif

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/test/singleton_instantiation_test",
      singleton_instantiation_test);
  g_test_add_func ("/test/thread_safety_instantiation_test",
      thread_safety_instantiation_test);
  g_test_add_func ("/test/session_mem_leak_test", session_mem_leak_test);

  return g_test_run ();
}
