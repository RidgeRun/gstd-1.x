#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/check/gstcheck.h>

#include "gstd_session.h"


GST_START_TEST (test_no_create)
{
  GstdObject *node;
  GstdReturnCode ret;
  GstdSession *test_session = gstd_session_new ("Test_session");

  /* Create pipeline to test no create cases */
  ret = gstd_get_by_uri (test_session, "/pipelines", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, "p0", "fakesrc ! fakesink");
  fail_if (ret);
  gst_object_unref (node);

  /* Test create at the pipeline level */
  ret = gstd_get_by_uri (test_session, "/pipelines/p0", &node);
  fail_if (ret);
  fail_if (NULL == node);

  ret = gstd_object_create (node, NULL, NULL);
  fail_if (GSTD_NO_CREATE != ret);

  gst_object_unref(node);
  gst_object_unref(test_session);
}
GST_END_TEST;

static Suite *
gstd_no_create_suite (void)
{
  Suite *suite = suite_create ("gstd_no_create");
  TCase *tc = tcase_create ("general");

  suite_add_tcase (suite, tc);
  tcase_add_test (tc, test_no_create);

  return suite;
}

GST_CHECK_MAIN (gstd_no_create);
