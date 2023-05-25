# Define source code
gstc_sources = [
  'libgstc_assert.c',
  'libgstc.c',
  'libgstc_json.c',
  'libgstc_thread.c',
  'libgstc_socket.c'
]

gstc_headers = [
  'libgstc_assert.h',
  'libgstc.h',
  'libgstc_json.h',
  'libgstc_socket.h',
  'libgstc_thread.h'
]

## Build and install library and pkgconfig file
# Build library
gstc_lib = library('gstc-@0@'.format(apiversion),
  gstc_sources,
  c_args : gst_c_args,
  version : gstd_version,
  include_directories : configinc,
  install : true,
  install_dir : lib_install_dir,
  dependencies : [libgstc_deps],
)
# Install library header file
install_headers('libgstc.h')

# Generate pkgconfig file
pkgconfig.generate(gstc_lib, description : 'GStreamer Client library to control Gstd')

# Define the library as an internal dependency to the current build
lib_gstc_inc = include_directories('.')
lib_gstc_dep = declare_dependency(
  link_with: gstc_lib,
  dependencies : [libgstc_deps],
  include_directories : [lib_gstc_inc]
)

lib_gstc_dir = meson.current_source_dir()
