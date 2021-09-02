lib_LTLIBRARIES = libgstd-@GSTD_API_VERSION@.la

gstdincludedir = $(includedir)/libgstd
gstdinclude_HEADERS = libgstd.h

libgstd_@GSTD_API_VERSION@_la_SOURCES =             \
             libgstd.c                              \
             libgstd_assert.c                       \
             gstd_session.c                         \
             gstd_object.c                          \
             gstd_pipeline.c                        \
             gstd_element.c                         \
             gstd_list.c                            \
             gstd_ipc.c                             \
             gstd_tcp.c                             \
             gstd_http.c                            \
             gstd_icreator.c                        \
             gstd_iformatter.c                      \
             gstd_pipeline_creator.c                \
             gstd_no_creator.c                      \
             gstd_json_builder.c                    \
             gstd_ideleter.c                        \
             gstd_pipeline_deleter.c                \
             gstd_no_deleter.c                      \
             gstd_debug.c                           \
             gstd_event_creator.c                   \
             gstd_event_factory.c                   \
             gstd_pipeline_bus.c                    \
             gstd_ireader.c                         \
             gstd_property_reader.c                 \
             gstd_no_reader.c                       \
             gstd_list_reader.c                     \
             gstd_property.c                        \
             gstd_property_int.c                    \
             gstd_property_string.c                 \
             gstd_property_boolean.c                \
             gstd_property_array.c                  \
             gstd_iupdater.c                        \
             gstd_no_updater.c                      \
             gstd_property_enum.c                   \
             gstd_property_flags.c                  \
             gstd_event_handler.c                   \
             gstd_bus_msg.c                         \
             gstd_bus_msg_simple.c                  \
             gstd_bus_msg_notify.c                  \
             gstd_bus_msg_state_changed.c           \
             gstd_msg_reader.c                      \
             gstd_msg_type.c                        \
             gstd_bus_msg_qos.c                     \
             gstd_return_codes.c                    \
             gstd_state.c                           \
             gstd_parser.c                          \
             gstd_log.c                             \
             gstd_bus_msg_stream_status.c           \
             gstd_bus_msg_element.c                 \
             gstd_signal.c                          \
             gstd_callback.c                        \
             gstd_signal_reader.c                   \
             gstd_socket.c                          \
             gstd_unix.c                            \
             gstd_signal_list.c

libgstd_@GSTD_API_VERSION@_la_CFLAGS =                         \
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
