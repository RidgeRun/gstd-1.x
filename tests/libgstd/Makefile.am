TESTS =                                     \
        test_libgstd_init                   \
        test_libgstd_create                 \
        test_libgstd_read                   \
        test_libgstd_update                 \
        test_libgstd_delete 

check_PROGRAMS = $(TESTS)

AM_CFLAGS =  $(GSTD_CFLAGS)                                   \
             $(GST_CFLAGS)                                    \
             -I$(top_srcdir)/gstd/                            \
             $(GIO_CFLAGS)                                    \
             $(GIO_UNIX_CFLAGS)                               \
             $(LIBSOUP_CFLAGS)                                \
             $(GJSON_CFLAGS)                                  \
             -DGSTD_LOG_STATE_DIR=\"$(GSTD_LOG_STATE_DIR)\"   \
             -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\"   \
             $(JANSSON_CFLAGS) -pthread                       \
             -I$(top_srcdir)/libgstd

AM_LDFLAGS =             \
    $(GSTD_LIBS)         \
    $(GST_LIBS)          \
    $(GIO_LIBS)          \
    $(GJSON_LIBS)        \
    $(LIBSOUP_LIBS)      \
    $(JANSSON_LIBS)
LDADD = $(top_srcdir)/libgstd/libgstd-@GSTD_API_VERSION@.la
