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

#include "libgstc.h"

GstcStatus
gstc_json_get_int (const char * json, const char * name, int * out);

GstcStatus
gstc_json_is_null (const char * json, const char * name, int * out);

/**
 * gstc_json_get_child_char_array:
 * @json: Json as a cstring with the data to be searched for
 * @parent_name: element name that is parent to the array
 * @array_name: name of the array
 * @element_name: name of the elements inside the array
 * @out: pointer to array of char*, this memory is allocated by this function
 * but needs to be freed by the client
 * @array_lenght: number of elements in out array
 *
 * Returns all the elements that have element_name that belongs to array_name,
 * which in turn is the child of parent_name
 *
 * Returns: GstcStatus indicating success, null argument, type error,
 * malformed string, unfound element
 */
GstcStatus
gstc_json_get_child_char_array(const char *json, const char* parent_name,
  const char* array_name, const char *element_name, char **out[], int *array_lenght);
