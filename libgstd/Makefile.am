lib_LTLIBRARIES = libgstd-@GSTD_API_VERSION@.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = libgstd.h

libgstd_@GSTD_API_VERSION@_la_SOURCES = \
	libgstd.c			\
	libgstd_assert.c

libgstd_@GSTD_API_VERSION@_la_CFLAGS =  $(GST_CFLAGS)					\
			 -I$(top_srcdir)/gstd/			\
			 $(GIO_CFLAGS)					\
			 $(GIO_UNIX_CFLAGS)				\
			 $(LIBSOUP_CFLAGS)				\
			 $(GJSON_CFLAGS)				\
			 -DGSTD_LOG_STATE_DIR=\"$(GSTD_LOG_STATE_DIR)\"	\
			 -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\" \
			 $(JANSSON_CFLAGS) -pthread

libgstd_@GSTD_API_VERSION@_la_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS) $(LIBSOUP_LIBS) $(JANSSON_LIBS) -pthread

LDADD = $(top_srcdir)/gstd/libgstd-core.la

noinst_HEADERS = \
	libgstd_assert.h

