TESTS = test_gstd_pipeline

check_PROGRAMS = $(TESTS)

test_gstd_pipeline_SOURCES = test_gstd_pipeline.c 

test_gstd_pipeline_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
test_gstd_pipeline_LDLAGS = $(GST_LIBS)
test_gstd_pipeline_LDADD = $(top_srcdir)/gstd/libgstd-core.la
