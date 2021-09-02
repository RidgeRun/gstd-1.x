gstd_name = 'gstd'

# Application source files
gstd_src_files = [
  'gstd_daemon.c',
  'gstd.c'
]

libgstd_src_files = [
  '../libgstd/gstd_object.c',
  '../libgstd/gstd_pipeline.c',
  '../libgstd/gstd_element.c',
  '../libgstd/gstd_list.c',
  '../libgstd/gstd_ipc.c',
  '../libgstd/gstd_tcp.c',
  '../libgstd/gstd_http.c',
  '../libgstd/gstd_icreator.c',
  '../libgstd/gstd_iformatter.c',
  '../libgstd/gstd_pipeline_creator.c',
  '../libgstd/gstd_no_creator.c',
  '../libgstd/gstd_json_builder.c',
  '../libgstd/gstd_ideleter.c',
  '../libgstd/gstd_pipeline_deleter.c',
  '../libgstd/gstd_no_deleter.c',
  '../libgstd/gstd_debug.c',
  '../libgstd/gstd_event_creator.c',
  '../libgstd/gstd_event_factory.c',
  '../libgstd/gstd_pipeline_bus.c',
  '../libgstd/gstd_ireader.c',
  '../libgstd/gstd_property_reader.c',
  '../libgstd/gstd_no_reader.c',
  '../libgstd/gstd_list_reader.c',
  '../libgstd/gstd_property.c',
  '../libgstd/gstd_property_int.c',
  '../libgstd/gstd_property_string.c',
  '../libgstd/gstd_property_boolean.c',
  '../libgstd/gstd_property_array.c',
  '../libgstd/gstd_iupdater.c',
  '../libgstd/gstd_no_updater.c',
  '../libgstd/gstd_property_enum.c',
  '../libgstd/gstd_property_flags.c',
  '../libgstd/gstd_event_handler.c',
  '../libgstd/gstd_bus_msg.c',
  '../libgstd/gstd_bus_msg_simple.c',
  '../libgstd/gstd_bus_msg_notify.c',
  '../libgstd/gstd_bus_msg_state_changed.c',
  '../libgstd/gstd_msg_reader.c',
  '../libgstd/gstd_msg_type.c',
  '../libgstd/gstd_bus_msg_qos.c',
  '../libgstd/gstd_return_codes.c',
  '../libgstd/gstd_state.c',
  '../libgstd/gstd_parser.c',
  '../libgstd/gstd_log.c',
  '../libgstd/gstd_bus_msg_stream_status.c',
  '../libgstd/gstd_bus_msg_element.c',
  '../libgstd/gstd_signal.c',
  '../libgstd/gstd_signal_list.c',
  '../libgstd/gstd_callback.c',
  '../libgstd/gstd_signal_reader.c',
  '../libgstd/gstd_session.c',
  '../libgstd/gstd_socket.c',
  '../libgstd/gstd_unix.c',
  '../libgstd/libgstd.c'
]

# Create gstd application
exe_name = '@0@-@1@'.format(gstd_name, apiversion)
executable(exe_name,
  [gstd_src_files, libgstd_src_files],
  install: true,
  include_directories : [configinc, libgstd_inc_dir],
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
