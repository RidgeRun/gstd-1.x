lib_LTLIBRARIES = libgstd-@GSTD_API_VERSION@.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS =                               \
             gstd.h                                 \
             gstd_return_codes.h

libgstd_@GSTD_API_VERSION@_la_SOURCES =             \
             gstd_action.c                          \
             gstd_bus_msg.c                         \
             gstd_bus_msg_element.c                 \
             gstd_bus_msg_notify.c                  \
             gstd_bus_msg_qos.c                     \
             gstd_bus_msg_simple.c                  \
             gstd_bus_msg_state_changed.c           \
             gstd_bus_msg_stream_status.c           \
             gstd_callback.c                        \
             gstd_debug.c                           \
             gstd_element.c                         \
             gstd_event_creator.c                   \
             gstd_event_factory.c                   \
             gstd_event_handler.c                   \
             gstd_http.c                            \
             gstd_icreator.c                        \
             gstd_ideleter.c                        \
             gstd_iformatter.c                      \
             gstd_ipc.c                             \
             gstd_ireader.c                         \
             gstd_iupdater.c                        \
             gstd_json_builder.c                    \
             gstd_list.c                            \
             gstd_list_reader.c                     \
             gstd_log.c                             \
             gstd_msg_reader.c                      \
             gstd_msg_type.c                        \
             gstd_no_creator.c                      \
             gstd_no_deleter.c                      \
             gstd_no_reader.c                       \
             gstd_no_updater.c                      \
             gstd_object.c                          \
             gstd_parser.c                          \
             gstd_pipeline.c                        \
             gstd_pipeline_bus.c                    \
             gstd_pipeline_creator.c                \
             gstd_pipeline_deleter.c                \
             gstd_property.c                        \
             gstd_property_array.c                  \
             gstd_property_boolean.c                \
             gstd_property_enum.c                   \
             gstd_property_flags.c                  \
             gstd_property_int.c                    \
             gstd_property_reader.c                 \
             gstd_property_string.c                 \
             gstd_return_codes.c                    \
             gstd_session.c                         \
             gstd_signal.c                          \
             gstd_signal_list.c                     \
             gstd_signal_reader.c                   \
             gstd_socket.c                          \
             gstd_state.c                           \
             gstd_tcp.c                             \
             gstd_unix.c                            \
             libgstd.c

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

noinst_HEADERS =                                   \
             gstd_action.h                         \
             gstd_bus_msg.h                        \
             gstd_bus_msg_element.h                \
             gstd_bus_msg_notify.h                 \
             gstd_bus_msg_qos.h                    \
             gstd_bus_msg_simple.h                 \
             gstd_bus_msg_state_changed.h          \
             gstd_bus_msg_stream_status.h          \
             gstd_callback.h                       \
             gstd_debug.h                          \
             gstd_element.h                        \
             gstd_event_creator.h                  \
             gstd_event_factory.h                  \
             gstd_event_handler.h                  \
             gstd_http.h                           \
             gstd_icreator.h                       \
             gstd_ideleter.h                       \
             gstd_iformatter.h                     \
             gstd_ipc.h                            \
             gstd_ireader.h                        \
             gstd_iupdater.h                       \
             gstd_json_builder.h                   \
             gstd_list.h                           \
             gstd_list_reader.h                    \
             gstd_log.h                            \
             gstd_msg_reader.h                     \
             gstd_msg_type.h                       \
             gstd_no_creator.h                     \
             gstd_no_deleter.h                     \
             gstd_no_reader.h                      \
             gstd_no_updater.h                     \
             gstd_parser.h                         \
             gstd_pipeline.h                       \
             gstd_pipeline_bus.h                   \
             gstd_pipeline_creator.h               \
             gstd_pipeline_deleter.h               \
             gstd_property.h                       \
             gstd_property_array.h                 \
             gstd_property_boolean.h               \
             gstd_property_enum.h                  \
             gstd_property_flags.h                 \
             gstd_property_int.h                   \
             gstd_property_reader.h                \
             gstd_property_string.h                \
             gstd_session.h                        \
             gstd_signal.h                         \
             gstd_signal_list.h                    \
             gstd_signal_reader.h                  \
             gstd_socket.h                         \
             gstd_state.h                          \
             gstd_tcp.h                            \
             gstd_unix.h
