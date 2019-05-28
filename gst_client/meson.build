gst_client_name = 'gst-client'

gst_client_src_files = [
  'gst_client.c'
]

# Create gst_client application
exe_name = '@0@-@1@'.format(gst_client_name, apiversion)
executable(exe_name,
  gst_client_src_files,
  install: true,
  include_directories : [configinc],
  dependencies : gst_client_deps,
  c_args: gst_c_args,
)
