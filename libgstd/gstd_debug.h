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
#ifndef __GSTD_DEBUG_H__
#define __GSTD_DEBUG_H__

#include <gst/gst.h>

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

G_BEGIN_DECLS
#define GSTD_TYPE_DEBUG \
  (gstd_debug_get_type())
#define GSTD_DEBUG(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_DEBUG,GstdDebug))
#define GSTD_DEBUG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_DEBUG,GstdDebugClass))
#define GSTD_IS_DEBUG(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_DEBUG))
#define GSTD_IS_DEBUG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_DEBUG))
#define GSTD_DEBUG_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_DEBUG, GstdDebugClass))
typedef struct _GstdDebug GstdDebug;
typedef struct _GstdDebugClass GstdDebugClass;
GType gstd_debug_get_type (void);

/**
 * gstd_debug_new: (constructor)
 * 
 * Creates a new object to handle debugging options.
 *
 * Returns: (transfer full) (nullable): A new #GstdDebug. Free after
 * usage using g_object_unref()
 */
GstdDebug *gstd_debug_new (void);


#endif // __GSTD_DEBUG_H__
