# Standalone application
bin_PROGRAMS = gstd

gstd_SOURCES =  gstd.c gstd_daemon.c

gstd_CFLAGS =                                                 \
             $(GST_CFLAGS)                                    \
	      -I$(top_srcdir)/libgstd/                          \
	      $(LIBD_CFLAGS)                                    \
	      -DGSTD_LOG_STATE_DIR=\"$(GSTD_LOG_STATE_DIR)\"    \
	      -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\"

gstd_LDFLAGS = $(GST_LIBS)                    \
	       $(LIBD_LIBS)                     \
	       -Wl,-rpath -Wl,$(libdir)

gstd_LDADD = $(top_builddir)/libgstd/libgstd-1.0.la

noinst_HEADERS = gstd_daemon.h

# Create an open area for our pid and log files
install-data-hook: $(DESTDIR)$(GSTD_RUN_STATE_DIR) $(DESTDIR)$(GSTD_LOG_STATE_DIR)

$(DESTDIR)@GSTD_RUN_STATE_DIR@ $(DESTDIR)@GSTD_LOG_STATE_DIR@:
	$(MKDIR_P) $@
	chmod a+w $@
