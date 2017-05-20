#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/check/gstcheck.h>

#include "gstd_session.h"

#define P0_DESCRIPTION "videotestsrc ! autovideosink"
#define P0_NAME "p0"

GST_START_TEST (test_successful_create)
{
  GstdObject *node;
  GstdReturnCode ret;
  GstdSession *test_session = gstd_session_new ("Test_session");

  ret = gstd_get_by_uri (test_session, "/pipelines", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, P0_NAME, P0_DESCRIPTION);
  fail_if (0 != ret);

  gst_object_unref(node);
  gst_object_unref(test_session);
}
GST_END_TEST;

static Suite *
gstd_create_suite (void)
{
  Suite *suite = suite_create ("gstd_create");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);
  tcase_add_test (tc, test_successful_create);

  return suite;
}

GST_CHECK_MAIN (gstd_create);
