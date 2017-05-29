bin_PROGRAMS = gst-client-@GSTD_API_VERSION@

gst_client_@GSTD_API_VERSION@_SOURCES = gst_client.c
gst_client_@GSTD_API_VERSION@_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS) $(GJSON_CFLAGS) $(READLINE_CFLAGS)
gst_client_@GSTD_API_VERSION@_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS) $(READLINE_LIBS)

install-exec-hook:
	$(RM) $(DESTDIR)$(bindir)/gstd-client $(DESTDIR)$(bindir)/gst-client
	$(LN_S) $(DESTDIR)$(bindir)/gst-client-@GSTD_API_VERSION@ \
           $(DESTDIR)$(bindir)/gstd-client
	$(LN_S) $(DESTDIR)$(bindir)/gst-client-@GSTD_API_VERSION@ \
           $(DESTDIR)$(bindir)/gst-client

uninstall-hook:
	$(RM) $(DESTDIR)$(bindir)/gstd-client $(DESTDIR)$(bindir)/gst-client
