TESTS = test_gstd_session \
	test_gstd_pipeline_create \
	test_gstd_state

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
AM_LDFLAGS = $(GST_LIBS)
LDADD = $(top_srcdir)/gstd/libgstd-core.la
