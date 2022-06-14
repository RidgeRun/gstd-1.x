bin_PROGRAMS = gst-client-@GSTD_API_VERSION@

gst_client_@GSTD_API_VERSION@_SOURCES = gst_client.c
gst_client_@GSTD_API_VERSION@_CFLAGS = \
    $(GSTD_CFLAGS)                     \
    $(GST_CFLAGS)                      \
    $(GIO_CFLAGS)                      \
    $(GIO_UNIX_CFLAGS)                 \
    $(GJSON_CFLAGS)                    \
    $(LIBEDIT_CFLAGS)                  \
    -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\"

gst_client_@GSTD_API_VERSION@_LDFLAGS = \
    $(GSTD_LIBS)                        \
    $(GST_LIBS)                         \
    $(GIO_LIBS)                         \
    $(GIO_UNIX_LIBS)                    \
    $(GJSON_LIBS)                       \
    $(LIBEDIT_LIBS)

install-exec-hook:
	$(RM) $(DESTDIR)$(bindir)/gstd-client $(DESTDIR)$(bindir)/gst-client
	$(LN_S) $(DESTDIR)$(bindir)/gst-client-@GSTD_API_VERSION@ \
           $(DESTDIR)$(bindir)/gstd-client
	$(LN_S) $(DESTDIR)$(bindir)/gst-client-@GSTD_API_VERSION@ \
           $(DESTDIR)$(bindir)/gst-client

uninstall-hook:
	$(RM) $(DESTDIR)$(bindir)/gstd-client $(DESTDIR)$(bindir)/gst-client
