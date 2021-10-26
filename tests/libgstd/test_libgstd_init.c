/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2021 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <gst/check/gstcheck.h>
#include "gstd.h"

GST_START_TEST (test_gstd_new_success)
{
  GstD *manager;
  GstdReturnCode ret = GSTD_EOK;

  ret = gstd_new (&manager, 0, NULL);

  assert_equals_int (GSTD_EOK, ret);
  fail_if (NULL == manager);
  fail_if (!gst_is_initialized ());

  gstd_free (manager);
}

GST_END_TEST;


static Suite *
gstd_new_suite (void)
{
  Suite *suite = suite_create ("gstd_new");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);

  tcase_add_test (tc, test_gstd_new_success);

  return suite;
}

GST_CHECK_MAIN (gstd_new);
