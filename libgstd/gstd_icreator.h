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

#ifndef __GSTD_ICREATOR_H__
#define __GSTD_ICREATOR_H__

#include <gst/gst.h>
#include <gstd_return_codes.h>

G_BEGIN_DECLS
#define GSTD_TYPE_ICREATOR                (gstd_icreator_get_type ())
#define GSTD_ICREATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_ICREATOR, GstdICreator))
#define GSTD_IS_ICREATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_ICREATOR))
#define GSTD_ICREATOR_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_ICREATOR, GstdICreatorInterface))
typedef struct _GstdICreator GstdICreator;
typedef struct _GstdICreatorInterface GstdICreatorInterface;

// Avoid cyclic dependecies by forward declaration
typedef struct _GstdObject GstdObject;

struct _GstdICreatorInterface
{
  GTypeInterface parent;

  GstdReturnCode (*create) (GstdICreator * self, const gchar * name,
      const gchar * description, GstdObject ** out);
};

GType gstd_icreator_get_type (void);

GstdReturnCode gstd_icreator_create (GstdICreator * self, const gchar * name,
    const gchar * description, GstdObject ** out);

G_END_DECLS
#endif // __GSTD_ICREATOR_H__
