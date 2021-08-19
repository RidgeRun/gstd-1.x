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
