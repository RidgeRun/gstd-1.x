/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gstd_session.h"
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void singleton_instantiation_test(){
  GstdSession *temp1 = NULL, *temp2 = NULL;
  gchar *name1, *name2;
  GPid pid1 = 1, pid2 = 2;
  temp1 = gstd_session_new (NULL, 8001);
  g_assert_true (temp1 != temp2);
  temp2 = gstd_session_new ("Session0", 8002);
  g_assert_true (temp1 == temp2);
  g_print("GstdSession temp1 ptr: %p, GstdSession temp2 ptr: %p\n", temp1, temp2);
  g_object_get(temp1, "pid", &pid1, NULL);
  g_object_get(temp2, "pid", &pid2, NULL);
  g_print("PID 1: %d PID 2: %d  \n", pid1, pid2);
  g_object_get(temp1, "name", &name1, NULL);
  g_object_get(temp2, "name", &name2, NULL);
  g_print("Name 1: %s Name 2: %s  \n", name1, name2);
  g_assert_cmpint(pid1, == , pid1);
  g_object_unref(temp1);
  g_object_unref(temp2);
  g_free(name1);
  g_free(name2);
}

void session_mem_leak_test(){
    gint reps = 10;
    gint i;
    for (i = 0; i < reps; ++i){
        singleton_instantiation_test();
    }
}



void* instantiate_session_singleton(gpointer address){
  g_print("Array Adress: %p, ", address);
  GstdSession **sessionAdress = (GstdSession **) address;
  *sessionAdress = gstd_session_new("SessionTest", 8002);
  g_print("GstdSession ptr: %p \n", *sessionAdress);
  return NULL;
}

void thread_safety_instantiation_test()
{

  gint reps = 10;
  gint num_threads = 3;
  gint i, j, r;
  GThread *threads[num_threads];
  GstdSession *sessions[num_threads];

  //We need at least 2 threads to test
  g_assert_true(num_threads > 2);

  for (r = 0; r < reps; ++r)
  {
    //spawn threads
    for (i = 0; i < num_threads; ++i)
    {
      threads[i] = g_thread_new("thread", instantiate_session_singleton, (gpointer)&sessions[i]);
    }

    //wait for threads to finish
    g_thread_join(threads[0]);
    for (j = 1; j < num_threads; ++j)
    {
      g_thread_join(threads[j]);
      //Check all singletons are the same
      g_print("GstdSession ptr %d: %p, GstdSession ptr %d: %p \n", j-1, sessions[j-1], j, sessions[j]);
      g_assert_true(sessions[j-1] == sessions[j]);
      g_object_unref(sessions[j-1]);
    }
    g_object_unref(sessions[num_threads-1]);
  }
}

gint
main (gint argc, gchar *argv[])
{
  /* Initialize GLib, deprecated in 2.36 */
  #if !GLIB_CHECK_VERSION(2,36,0)
  g_type_init();
  #endif

  g_test_init (&argc, &argv, NULL);
 
  g_test_add_func ("/test/singleton_instantiation_test", singleton_instantiation_test);
  g_test_add_func ("/test/thread_safety_instantiation_test", thread_safety_instantiation_test);
  g_test_add_func ("/test/session_mem_leak_test", session_mem_leak_test);
  
  return g_test_run ();
}