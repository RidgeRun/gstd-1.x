# Define source code
gstc_sources = [
  'libgstd.c',
]

gstc_headers = [
  'libgstd.h',
]

## Build and install library and pkgconfig file
# Build library
gstd_lib = library('gstd-@0@'.format(apiversion),
  gstc_sources,
  c_args : gst_c_args,
  version : gstd_version,
  include_directories : configinc,
  install : true,
  install_dir : lib_install_dir,
  dependencies : [libgstc_deps],
)
# Install library header file
install_headers('libgstd.h')

# Generate pkgconfig file
pkgconfig.generate(gstd_lib, description : 'GStreamer Manager library to control Gstd')

# Define the library as an internal dependency to the current build
lib_gstd_manager_dep = declare_dependency(link_with: gstd_lib,
  dependencies : [libgstc_deps])

lib_gstd_manager_dir = meson.current_source_dir()
