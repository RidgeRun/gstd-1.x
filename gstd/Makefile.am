# Core library if user wants to use as code base for its own application
lib_LTLIBRARIES = libgstd-core.la

libgstd_core_la_SOURCES = gstd.c 		\
			  gstd_object.c		\
	                  gstd_pipeline.c 	
#			  gstd_element.c

libgstd_core_la_CFLAGS = $(GST_CFLAGS)
libgstd_core_la_LDFLAGS = $(GST_LIBS)

# Standalone application that used libgstd-core
bin_PROGRAMS = gstd

gstd_SOURCES = main.c
gstd_CFLAGS = $(GST_CFLAGS)
gstd_LDFLAGS = $(GST_LIBS)
gstd_LDADD = libgstd-core.la

include_HEADERS = gstd.h 		\
		  gstd_object.h		\
		  gstd_return_codes.h 	\
		  gstd_pipeline.h	\
		  gstd_element.h

noinst_HEADERS = gstd_debug.h
