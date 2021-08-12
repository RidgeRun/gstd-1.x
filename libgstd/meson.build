libgstd_src_files = [
  '../gstd/gstd_object.c',
  '../gstd/gstd_pipeline.c',
  '../gstd/gstd_element.c',
  '../gstd/gstd_list.c',
  '../gstd/gstd_ipc.c',
  '../gstd/gstd_tcp.c',
  '../gstd/gstd_http.c',
  '../gstd/gstd_icreator.c',
  '../gstd/gstd_iformatter.c',
  '../gstd/gstd_pipeline_creator.c',
  '../gstd/gstd_no_creator.c',
  '../gstd/gstd_json_builder.c',
  '../gstd/gstd_ideleter.c',
  '../gstd/gstd_pipeline_deleter.c',
  '../gstd/gstd_no_deleter.c',
  '../gstd/gstd_debug.c',
  '../gstd/gstd_event_creator.c',
  '../gstd/gstd_event_factory.c',
  '../gstd/gstd_pipeline_bus.c',
  '../gstd/gstd_ireader.c',
  '../gstd/gstd_property_reader.c',
  '../gstd/gstd_no_reader.c',
  '../gstd/gstd_list_reader.c',
  '../gstd/gstd_property.c',
  '../gstd/gstd_property_int.c',
  '../gstd/gstd_property_string.c',
  '../gstd/gstd_property_boolean.c',
  '../gstd/gstd_property_array.c',
  '../gstd/gstd_iupdater.c',
  '../gstd/gstd_no_updater.c',
  '../gstd/gstd_property_enum.c',
  '../gstd/gstd_property_flags.c',
  '../gstd/gstd_event_handler.c',
  '../gstd/gstd_bus_msg.c',
  '../gstd/gstd_bus_msg_simple.c',
  '../gstd/gstd_bus_msg_notify.c',
  '../gstd/gstd_bus_msg_state_changed.c',
  '../gstd/gstd_msg_reader.c',
  '../gstd/gstd_msg_type.c',
  '../gstd/gstd_bus_msg_qos.c',
  '../gstd/gstd_return_codes.c',
  '../gstd/gstd_state.c',
  '../gstd/gstd_parser.c',
  '../gstd/gstd_log.c',
  '../gstd/gstd_bus_msg_stream_status.c',
  '../gstd/gstd_bus_msg_element.c',
  '../gstd/gstd_signal.c',
  '../gstd/gstd_signal_list.c',
  '../gstd/gstd_callback.c',
  '../gstd/gstd_signal_reader.c',
  '../gstd/gstd_session.c',
  '../gstd/gstd_socket.c',
  '../gstd/gstd_unix.c'
]

gstd_src = [
  'libgstd_assert.c',
  'libgstd.c'
]

gstd_headers = [
  'libgstd.h',
]

gstd_lib_manager = library('gstd-@0@'.format(apiversion),
  [libgstd_src_files, gstd_src],
  c_args : gst_d_args,
  version : gstd_version,
  include_directories : [configinc, gstd_inc_dir],
  install : true,
  install_dir : lib_install_dir,
  dependencies : [libgstd_deps],
)

install_headers(gstd_headers)

# Generate pkgconfig file
pkgconfig.generate(gstd_lib_manager, description : 'GStreamer Manager library to control Gstd')

# Define the library as an internal dependency to the current build
lib_gstd_manager_dep = declare_dependency(link_with: gstd_lib_manager,
  dependencies : [libgstd_deps])

lib_gstd_manager_dir = meson.current_source_dir()