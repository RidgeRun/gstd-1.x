gstd_name = 'gstd'

# Application source files
gstd_src_files = [
  'gstd_daemon.c',
  'gstd.c'
]

libgstd_src_files = [
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
  install_rpath: get_option('prefix') + '/' + get_option('libdir'),
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
  exe_name,
  get_option('bindir') +'/'+ gstd_name
)
