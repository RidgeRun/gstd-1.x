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

#ifndef __LIBGSTC_ASSERT_H__
#define __LIBGSTC_ASSERT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LIBGSTC_CHECKS_CRITICAL
#define gstc_abort() abort()
#else
#define gstc_abort()
#endif

#ifdef LIBGST_CHECKS_DISABLE
#define gstc_assert_and_ret(cond)
#define gstc_assert_and_ret_val(cond, val)
#define gstc_assert(cond)
#else

#define gstc_assert_and_ret(cond)					\
  _gstc_assert ((cond), #cond, __FILE__, __FUNCTION__, __LINE__);	\
  if (!(cond)) return

#define gstc_assert_and_ret_val(cond, val)				\
  _gstc_assert ((cond), #cond, __FILE__, __FUNCTION__, __LINE__);	\
  if (!(cond)) return (val)

#define gstc_assert(cond)						\
  _gstc_assert ((cond), #cond, __FILE__, __FUNCTION__, __LINE__)

void
_gstc_assert (int cond, const char *scond, const char *file,
    const char *function, int line);

#endif // LIBGSTC_CHECKS_DISABLE

#ifdef __cplusplus
}
#endif

#endif // __LIBGSTC_ASSERT_H__
