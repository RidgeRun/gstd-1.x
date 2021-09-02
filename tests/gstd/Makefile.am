TESTS = test_gstd_pipeline_create 	\
	test_gstd_no_create 		\
	test_gstd_state

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/libgstd/
AM_LDFLAGS = $(GST_LIBS)
LDADD = $(top_srcdir)/libgstd/libgstd-1.0.la
