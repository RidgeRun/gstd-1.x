lib_LTLIBRARIES = libgstd-@GSTD_API_VERSION@.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = libgstd.h

libgstd_@GSTD_API_VERSION@_la_SOURCES =             \
             libgstd.c                              \
             libgstd_assert.c                       \
             ../gstd/gstd_session.c                 \
             ../gstd/gstd_object.c                  \
             ../gstd/gstd_pipeline.c                \
             ../gstd/gstd_element.c                 \
             ../gstd/gstd_list.c                    \
             ../gstd/gstd_ipc.c                     \
             ../gstd/gstd_tcp.c                     \
             ../gstd/gstd_http.c                    \
             ../gstd/gstd_icreator.c                \
             ../gstd/gstd_iformatter.c              \
             ../gstd/gstd_pipeline_creator.c        \
             ../gstd/gstd_no_creator.c              \
             ../gstd/gstd_json_builder.c            \
             ../gstd/gstd_ideleter.c                \
             ../gstd/gstd_pipeline_deleter.c        \
             ../gstd/gstd_no_deleter.c              \
             ../gstd/gstd_debug.c                   \
             ../gstd/gstd_event_creator.c           \
             ../gstd/gstd_event_factory.c           \
             ../gstd/gstd_pipeline_bus.c            \
             ../gstd/gstd_ireader.c                 \
             ../gstd/gstd_property_reader.c         \
             ../gstd/gstd_no_reader.c               \
             ../gstd/gstd_list_reader.c             \
             ../gstd/gstd_property.c                \
             ../gstd/gstd_property_int.c            \
             ../gstd/gstd_property_string.c         \
             ../gstd/gstd_property_boolean.c        \
             ../gstd/gstd_property_array.c          \
             ../gstd/gstd_iupdater.c                \
             ../gstd/gstd_no_updater.c              \
             ../gstd/gstd_property_enum.c           \
             ../gstd/gstd_property_flags.c          \
             ../gstd/gstd_event_handler.c           \
             ../gstd/gstd_bus_msg.c                 \
             ../gstd/gstd_bus_msg_simple.c          \
             ../gstd/gstd_bus_msg_notify.c          \
             ../gstd/gstd_bus_msg_state_changed.c   \
             ../gstd/gstd_msg_reader.c              \
             ../gstd/gstd_msg_type.c                \
             ../gstd/gstd_bus_msg_qos.c             \
             ../gstd/gstd_return_codes.c            \
             ../gstd/gstd_state.c                   \
             ../gstd/gstd_parser.c                  \
             ../gstd/gstd_log.c                     \
             ../gstd/gstd_bus_msg_stream_status.c   \
             ../gstd/gstd_bus_msg_element.c         \
             ../gstd/gstd_signal.c                  \
             ../gstd/gstd_callback.c                \
             ../gstd/gstd_signal_reader.c           \
             ../gstd/gstd_socket.c                  \
             ../gstd/gstd_unix.c                    \
             ../gstd/gstd_signal_list.c

libgstd_@GSTD_API_VERSION@_la_CFLAGS =                                   \
			 $(GST_CFLAGS)                                     \
			 -I$(top_srcdir)/gstd/                             \
			 $(GIO_CFLAGS)                                     \
			 $(GIO_UNIX_CFLAGS)                                \
			 $(LIBSOUP_CFLAGS)                                 \
			 $(GJSON_CFLAGS)                                   \
			 -DGSTD_LOG_STATE_DIR=\"$(GSTD_LOG_STATE_DIR)\"    \
			 -DGSTD_RUN_STATE_DIR=\"$(GSTD_RUN_STATE_DIR)\"    \
			 $(JANSSON_CFLAGS) -pthread

libgstd_@GSTD_API_VERSION@_la_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS) $(LIBSOUP_LIBS) $(JANSSON_LIBS) -pthread

noinst_HEADERS = \
	libgstd_assert.h
