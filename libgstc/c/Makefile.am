lib_LTLIBRARIES = libgstc-@GSTD_API_VERSION@.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = libgstc.h

libgstc_@GSTD_API_VERSION@_la_SOURCES = \
	libgstc.c			\
	libgstc_socket.c		\
	libgstc_assert.c		\
	libgstc_json.c			\
	libgstc_thread.c

libgstc_@GSTD_API_VERSION@_la_CFLAGS = $(JANSSON_CFLAGS) -pthread
libgstc_@GSTD_API_VERSION@_la_LDFLAGS = $(JANSSON_LIBS) -pthread

noinst_HEADERS = \
	libgstc_socket.h	\
	libgstc_assert.h	\
	libgstc_json.h		\
	libgstc_thread.h
