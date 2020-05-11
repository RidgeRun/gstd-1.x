# Core library if user wants to use as code base for its own application
lib_LTLIBRARIES = libgstd-core.la

libgstd_core_la_SOURCES = gstd_session.c		\
			  gstd_object.c			\
	                  gstd_pipeline.c 		\
			  gstd_element.c		\
			  gstd_list.c			\
			  gstd_ipc.c			\
			  gstd_tcp.c			\
			  gstd_http.c			\
			  gstd_icreator.c		\
			  gstd_iformatter.c		\
			  gstd_pipeline_creator.c	\
			  gstd_no_creator.c		\
			  gstd_json_builder.c		\
			  gstd_ideleter.c		\
			  gstd_pipeline_deleter.c	\
			  gstd_no_deleter.c		\
			  gstd_debug.c			\
			  gstd_event_creator.c		\
			  gstd_event_factory.c		\
			  gstd_pipeline_bus.c		\
			  gstd_ireader.c		\
			  gstd_property_reader.c	\
			  gstd_no_reader.c		\
			  gstd_list_reader.c		\
			  gstd_property.c		\
			  gstd_property_int.c		\
			  gstd_property_string.c	\
			  gstd_property_boolean.c	\
			  gstd_property_array.c		\
			  gstd_iupdater.c		\
			  gstd_no_updater.c		\
			  gstd_property_enum.c		\
			  gstd_property_flags.c		\
			  gstd_event_handler.c		\
			  gstd_bus_msg.c		\
			  gstd_bus_msg_simple.c		\
			  gstd_bus_msg_notify.c		\
			  gstd_bus_msg_state_changed.c	\
			  gstd_msg_reader.c		\
			  gstd_msg_type.c		\
			  gstd_bus_msg_qos.c		\
			  gstd_return_codes.c		\
			  gstd_state.c			\
			  gstd_parser.c			\
			  gstd_log.c			\
			  gstd_bus_msg_stream_status.c  \
			  gstd_bus_msg_element.c	\
			  gstd_signal.c			\
			  gstd_callback.c		\
			  gstd_signal_reader.c		\
			  gstd_socket.c			\
			  gstd_unix.c			\
			  gstd_signal_list.c

libgstd_core_la_CFLAGS = $(GST_CFLAGS)					\
			 $(GIO_CFLAGS)					\
			 $(GIO_UNIX_CFLAGS)				\
			 $(LIBSOUP_CFLAGS)					\
			 $(GJSON_CFLAGS)				\
			 -DGSTD_LOG_STATE_DIR=\"$(gstdlogstatedir)\"	\
			 -DGSTD_RUN_STATE_DIR=\"$(gstdrunstatedir)\"

libgstd_core_la_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS) $(LIBSOUP_LIBS)

# Standalone application that uses libgstd-core
bin_PROGRAMS = gstd

gstd_SOURCES =  gstd.c gstd_daemon.c

gstd_CFLAGS = $(GST_CFLAGS)					\
	      $(GIO_CFLAGS)					\
		  $(LIBSOUP_CFLAGS)					\
	      $(GIO_UNIX_CFLAGS)				\
	      $(GJSON_CFLAGS)					\
	      $(LIBD_CFLAGS)					\
	      -DGSTD_LOG_STATE_DIR=\"$(gstdlogstatedir)\"	\
	      -DGSTD_RUN_STATE_DIR=\"$(gstdrunstatedir)\"

gstd_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GIO_UNIX_LIBS) $(GJSON_LIBS) $(LIBD_LIBS) $(LIBSOUP_LIBS) -Wl,-rpath -Wl,$(libdir)
gstd_LDADD = libgstd-core.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = \
		  gstd_session.h		\
		  gstd_object.h			\
		  gstd_return_codes.h 		\
		  gstd_pipeline.h		\
		  gstd_element.h		\
		  gstd_list.h			\
		  gstd_ipc.h			\
		  gstd_tcp.h			\
		  gstd_http.h			\
		  gstd_icreator.h		\
		  gstd_iformatter.h		\
		  gstd_pipeline_creator.h	\
		  gstd_json_builder.h		\
		  gstd_no_creator.h		\
		  gstd_ideleter.h		\
		  gstd_pipeline_deleter.h	\
		  gstd_no_deleter.h		\
		  gstd_ireader.h		\
		  gstd_property_reader.h	\
		  gstd_no_reader.h		\
		  gstd_list_reader.h		\
		  gstd_property.h		\
		  gstd_property_int.h		\
		  gstd_property_string.h	\
		  gstd_property_boolean.h	\
		  gstd_property_array.h         \
		  gstd_iupdater.h		\
		  gstd_no_updater.h		\
		  gstd_property_enum.h		\
		  gstd_property_flags.h		\
		  gstd_event_handler.h		\
		  gstd_event_creator.h		\
		  gstd_bus_msg.h		\
		  gstd_bus_msg_simple.h		\
		  gstd_bus_msg_notify.h		\
		  gstd_bus_msg_state_changed.h	\
		  gstd_msg_reader.h		\
		  gstd_msg_type.h		\
		  gstd_bus_msg_qos.h		\
		  gstd_state.h			\
		  gstd_parser.h			\
		  gstd_log.h			\
		  gstd_bus_msg_stream_status.h	\
		  gstd_bus_msg_element.h	\
		  gstd_signal_list.h

noinst_HEADERS = gstd_daemon.h

# Create an open area for our pid and log files
install-data-hook: $(DESTDIR)$(gstdrunstatedir) $(DESTDIR)$(gstdlogstatedir)

$(DESTDIR)@gstdrunstatedir@ $(DESTDIR)@gstdlogstatedir@:
	$(MKDIR_P) $@
	chmod a+w $@
