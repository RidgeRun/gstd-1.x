# Standalone application
bin_PROGRAMS = gstd

gstd_SOURCES =  gstd.c gstd_daemon.c

gstd_CFLAGS =                                               \
             $(GST_CFLAGS)                                     \
	      -I$(top_srcdir)/libgstd/                          \
	      $(GIO_CFLAGS)                                     \
	      $(LIBSOUP_CFLAGS)                                 \
	      $(GIO_UNIX_CFLAGS)                                \
	      $(GJSON_CFLAGS)                                   \
	      $(LIBD_CFLAGS)                                    \
	      -DGSTD_LOG_STATE_DIR=\"$(GSTD_LOG_STATE_DIR)\"    \
	      -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\"

gstd_LDFLAGS = $(GST_LIBS)                  \
	       $(GIO_LIBS)                      \
	       $(GIO_UNIX_LIBS)                 \
	       $(GJSON_LIBS)                    \
	       $(LIBD_LIBS)                     \
	       $(LIBSOUP_LIBS)                  \
	       -Wl,-rpath -Wl,$(libdir)

gstd_LDADD = $(top_srcdir)/libgstd/libgstd-1.0.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS =                   \
		  gstd_session.h                \
		  gstd_object.h                 \
		  gstd_return_codes.h           \
		  gstd_pipeline.h               \
		  gstd_element.h                \
		  gstd_list.h                   \
		  gstd_ipc.h                    \
		  gstd_tcp.h                    \
		  gstd_http.h                   \
		  gstd_icreator.h               \
		  gstd_iformatter.h             \
		  gstd_pipeline_creator.h       \
		  gstd_json_builder.h           \
		  gstd_no_creator.h             \
		  gstd_ideleter.h               \
		  gstd_pipeline_deleter.h       \
		  gstd_no_deleter.h             \
		  gstd_ireader.h                \
		  gstd_property_reader.h        \
		  gstd_no_reader.h              \
		  gstd_list_reader.h            \
		  gstd_property.h               \
		  gstd_property_int.h           \
		  gstd_property_string.h        \
		  gstd_property_boolean.h       \
		  gstd_property_array.h         \
		  gstd_iupdater.h               \
		  gstd_no_updater.h             \
		  gstd_property_enum.h          \
		  gstd_property_flags.h         \
		  gstd_event_handler.h          \
		  gstd_event_creator.h          \
		  gstd_bus_msg.h                \
		  gstd_bus_msg_simple.h         \
		  gstd_bus_msg_notify.h         \
		  gstd_bus_msg_state_changed.h  \
		  gstd_msg_reader.h             \
		  gstd_msg_type.h               \
		  gstd_bus_msg_qos.h            \
		  gstd_state.h                  \
		  gstd_parser.h	                \
		  gstd_log.h                    \
		  gstd_bus_msg_stream_status.h  \
		  gstd_bus_msg_element.h        \
		  gstd_signal_list.h

noinst_HEADERS = gstd_daemon.h

# Create an open area for our pid and log files
install-data-hook: $(DESTDIR)$(GSTD_RUN_STATE_DIR) $(DESTDIR)$(GSTD_LOG_STATE_DIR)

$(DESTDIR)@GSTD_RUN_STATE_DIR@ $(DESTDIR)@GSTD_LOG_STATE_DIR@:
	$(MKDIR_P) $@
	chmod a+w $@
