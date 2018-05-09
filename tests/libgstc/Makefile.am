TESTS = libgstc_pipeline_create

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/libgstc
AM_LDFLAGS = $(GST_LIBS)

libgstc_pipeline_create_SOURCES = 	\
	test_libgstc_pipeline_create.c	\
	$(top_srcdir)/libgstc/libgstc.c
