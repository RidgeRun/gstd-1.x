gstd_name = 'gstd'
# Application source files
gstd_src_files = [
  'gstd_daemon.c',
  'gstd.c'
]
# Library source files
libgstd_src_files = [
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
  'gstd_return_codes.c',
  'gstd_state.c',
  'gstd_parser.c',
  'gstd_log.c',
  'gstd_bus_msg_stream_status.c',
  'gstd_bus_msg_element.c',
  'gstd_signal.c',
  'gstd_signal_list.c',
  'gstd_callback.c',
  'gstd_signal_reader.c',
  'gstd_session.c',
  'gstd_socket.c',
  'gstd_unix.c'
]

libgstd_header_files = [
  'gstd_bus_msg_element.h',
  'gstd_bus_msg.h',
  'gstd_bus_msg_simple.h',
  'gstd_bus_msg_notify.h',
  'gstd_bus_msg_qos.h',
  'gstd_bus_msg_state_changed.h',
  'gstd_bus_msg_stream_status.h',
  'gstd_callback.h',
  'gstd_daemon.h',
  'gstd_debug.h',
  'gstd_element.h',
  'gstd_event_creator.h',
  'gstd_event_factory.h',
  'gstd_event_handler.h',
  'gstd_icreator.h',
  'gstd_ideleter.h',
  'gstd_iformatter.h',
  'gstd_ipc.h',
  'gstd_ireader.h',
  'gstd_iupdater.h',
  'gstd_json_builder.h',
  'gstd_list.h',
  'gstd_list_reader.h',
  'gstd_log.h',
  'gstd_msg_reader.h',
  'gstd_msg_type.h',
  'gstd_no_creator.h',
  'gstd_no_deleter.h',
  'gstd_no_reader.h',
  'gstd_no_updater.h',
  'gstd_object.h',
  'gstd_parser.h',
  'gstd_pipeline_bus.h',
  'gstd_pipeline_creator.h',
  'gstd_pipeline_deleter.h',
  'gstd_pipeline.h',
  'gstd_property_array.h',
  'gstd_property_boolean.h',
  'gstd_property_enum.h',
  'gstd_property_flags.h',
  'gstd_property.h',
  'gstd_property_int.h',
  'gstd_property_reader.h',
  'gstd_property_string.h',
  'gstd_return_codes.h',
  'gstd_session.h',
  'gstd_signal.h',
  'gstd_signal_reader.h',
  'gstd_state.h',
  'gstd_tcp.h',
  'gstd_socket.h',
  'gstd_unix.h'
]

# Create a static library used to create gstd daemon and also is used for tests
gstd_lib = both_libraries('gstd-core',
  libgstd_src_files,
  c_args : gst_c_args,
  version : gstd_version,
  include_directories : configinc,
  dependencies : [gstd_deps],
  install : true,
  install_dir : lib_install_dir,
)

# Define the library as an internal dependency to the current build
lib_gstd_dep = declare_dependency(link_with: gstd_lib,
  dependencies : [gstd_deps])

# Create gstd application
exe_name = '@0@-@1@'.format(gstd_name, apiversion)
executable(exe_name,
  gstd_src_files,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_deps, lib_gstd_dep],
  c_args: gst_c_args,
)
lib_gstd_dir = meson.current_build_dir()

## Install gstd files
# Install library headers
# install_headers(libgstd_header_files)
# Install run and log directories

install_subdir('gstd',
  install_dir : runstatedir,
  strip_directory : true,
)

install_subdir('gstd',
  install_dir : logstatedir,
  strip_directory : true,
)

# Since install_subdir function doesn't change the mode of the directories installed
# Run a script at install time to change the mode of the directories
meson.add_install_script('gstd_chmod.sh','777', runstatedir)
meson.add_install_script('gstd_chmod.sh','777', logstatedir)

## Create symbolic link to gstd
meson.add_install_script('gstd_symbolic_link.sh',
  get_option('bindir') +'/'+ exe_name,
  get_option('bindir') +'/'+ gstd_name
)
