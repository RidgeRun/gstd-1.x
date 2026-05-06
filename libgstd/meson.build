gstd_headers = [
  'gstd.h',
  'gstd_return_codes.h',
]

# Common files needed for GstD and libGstD
gstd_src = [
  'gstd_action.c',
  'gstd_object.c',
  'gstd_pipeline.c',
  'gstd_element.c',
  'gstd_list.c',
  'gstd_ipc.c',
  'gstd_tcp.c',
  'gstd_http.c',
  'gstd_icreator.c',
  'gstd_iformatter.c',
  'gstd_pipeline_creator.c',
  'gstd_no_creator.c',
  'gstd_json_builder.c',
  'gstd_ideleter.c',
  'gstd_pipeline_deleter.c',
  'gstd_no_deleter.c',
  'gstd_debug.c',
  'gstd_event_creator.c',
  'gstd_event_factory.c',
  'gstd_pipeline_bus.c',
  'gstd_ireader.c',
  'gstd_property_reader.c',
  'gstd_no_reader.c',
  'gstd_list_reader.c',
  'gstd_property.c',
  'gstd_property_int.c',
  'gstd_property_string.c',
  'gstd_property_boolean.c',
  'gstd_property_array.c',
  'gstd_iupdater.c',
  'gstd_no_updater.c',
  'gstd_property_enum.c',
  'gstd_property_flags.c',
  'gstd_event_handler.c',
  'gstd_bus_msg.c',
  'gstd_bus_msg_simple.c',
  'gstd_bus_msg_notify.c',
  'gstd_bus_msg_state_changed.c',
  'gstd_msg_reader.c',
  'gstd_msg_type.c',
  'gstd_bus_msg_qos.c',
  'gstd_state.c',
  'gstd_parser.c',
  'gstd_bus_msg_stream_status.c',
  'gstd_bus_msg_element.c',
  'gstd_signal.c',
  'gstd_signal_list.c',
  'gstd_callback.c',
  'gstd_signal_reader.c',
  'gstd_session.c',
  'gstd_socket.c',
  'gstd_unix.c',
  'gstd_log.c',
]

libgstd_src = [
  'libgstd.c', 
  'gstd_return_codes.c',
]

gstd_lib_manager = library('gstd-@0@'.format(apiversion),
  [gstd_src, libgstd_src],
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
lib_gstd_inc = include_directories('.')
lib_gstd_dep = declare_dependency(
  link_with: gstd_lib_manager,
  dependencies : [libgstd_deps],
  include_directories : [lib_gstd_inc]
)
