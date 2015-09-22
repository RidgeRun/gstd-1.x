TESTS = test_gstd_pipeline \
	test_gstd_element

check_PROGRAMS = $(TESTS)

test_gstd_pipeline_SOURCES = test_gstd_pipeline.c 

test_gstd_pipeline_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
test_gstd_pipeline_LDLAGS = $(GST_LIBS)
test_gstd_pipeline_LDADD = $(top_srcdir)/gstd/libgstd-core.la

test_gstd_element_SOURCES = test_gstd_element.c 

test_gstd_element_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
test_gstd_element_LDLAGS = $(GST_LIBS)
test_gstd_element_LDADD = $(top_srcdir)/gstd/libgstd-core.la
