# Copy the file provided in input: to a file in the build directory with
# the name output: in the current directory.
types = configure_file(input : 'gstd.types',
  output : 'gstd.types',
  copy : true)

# Compiles and installs gtkdoc documentation into prefix/share/gtk-doc/html.
gnome.gtkdoc('gstd',
  main_xml : 'gstd-docs.xml',
  src_dir : ['@0@/gstd/'.format(meson.source_root()) ],
  mkdb_args : ['--xml-mode', '--output-format=xml' ],
  html_args : '--verbose',
  gobject_typesfile : types,
  dependencies : [gstd_deps] + [lib_gstd_dep],
  install : true
)
