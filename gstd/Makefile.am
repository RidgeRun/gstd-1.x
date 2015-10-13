# Core library if user wants to use as code base for its own application
lib_LTLIBRARIES = libgstd-core.la

libgstd_core_la_SOURCES = gstd_session.c	\
			  gstd_object.c		\
	                  gstd_pipeline.c 	\
			  gstd_element.c	\
			  gstd_list.c		\
			  gstd_tcp.c

libgstd_core_la_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS)
libgstd_core_la_LDFLAGS = $(GST_LIBS) $(GIO_LIBS)

# Standalone application that uses libgstd-core
bin_PROGRAMS = gstd

gstd_SOURCES = gstd.c
gstd_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS)
gstd_LDFLAGS = $(GST_LIBS) $(GIO_LIBS)
gstd_LDADD = libgstd-core.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = \
		  gstd.h 		\
		  gstd_session.h	\
		  gstd_object.h		\
		  gstd_return_codes.h 	\
		  gstd_pipeline.h	\
		  gstd_element.h	\
		  gstd_list.h		\
		  gstd_tcp.h

noinst_HEADERS = 
