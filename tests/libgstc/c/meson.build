# Tests ad condition when to skip the test
lib_gstc_tests = [
  ['test_libgstc_ping.c'],
  ['test_libgstc_pipeline_create.c'],
  ['test_libgstc_pipeline_delete.c'],
  ['test_libgstc_pipeline_pause.c'],
  ['test_libgstc_pipeline_play.c'],
  ['test_libgstc_pipeline_stop.c'],
  ['test_libgstc_pipeline_get_graph.c'],
  ['test_libgstc_json.c'],
  ['test_libgstc_element_get.c'],
  ['test_libgstc_element_set.c'],
  ['test_libgstc_pipeline_inject_eos.c'],
  ['test_libgstc_pipeline_bus_wait_async.c'],
  ['test_libgstc_pipeline_bus_wait.c'],
  ['test_libgstc_pipeline_flush_start.c'],
  ['test_libgstc_pipeline_flush_stop.c'],
  ['test_libgstc_pipeline_seek.c'],
  ['test_libgstc_debug.c'],
  ['test_libgstc_pipeline_list_elements.c'],
  ['test_libgstc_pipeline_list.c'],
  ['test_libgstc_pipeline_list_properties.c'],
  ['test_libgstc_pipeline_verbose.c'],
  ['test_libgstc_pipeline_get_state.c'],
  ['test_libgstc_pipeline_list_signals.c'],
  ['test_libgstc_pipeline_signal_connect.c'],
  ['test_libgstc_pipeline_signal_disconnect.c'],
]

# These are specials tests since is required to re-compile libgstc
lib_gstc_client = [
  ['test_libgstc_client.c', lib_gstc_dir + '/libgstc_assert.c', lib_gstc_dir + '/libgstc_thread.c', lib_gstc_dir + '/libgstc.c'],
  ['test_libgstc_socket.c', lib_gstc_dir + '/libgstc_assert.c', lib_gstc_dir + '/libgstc_thread.c', lib_gstc_dir + '/libgstc_socket.c'],
]

plugins_dir = []
# Define plugins path
if gst_dep.type_name() == 'pkgconfig'
   plugins_dir = [gst_dep.get_pkgconfig_variable('pluginsdir')]
endif

# Create environment object to  stores information about the environment
# variables set during tests.
# Define constant enviroment variable
env = environment()
env.set('GST_PLUGIN_SYSTEM_PATH_1_0', '')
env.set('CK_DEFAULT_TIMEOUT', '120')
env.set('GST_PLUGIN_PATH_1_0', [meson.build_root()] + plugins_dir)

# Build and run tests
foreach t : lib_gstc_tests
  fname = t[0]
  test_name = fname.split('.')[0].underscorify()
  has_flags = t.get(1,false)
  flags = t.get(2,'')

  if has_flags
    test_flags = gst_c_args + [flags]
  else
    test_flags = gst_c_args
  endif

  exe = executable(test_name, fname,
      c_args : test_flags,
      cpp_args : test_flags,
      include_directories : [configinc, lib_gstc_inc_dir],
      dependencies : [test_libgstc_deps,lib_gstc_dep],
  )
  # Define enviroment variable
  env.set('GST_REGISTRY', '@0@/@1@.registry'.format(meson.current_build_dir(), test_name))
  # Run tests
  test(test_name, exe, env: env, timeout : 60)

endforeach


# Build and run special tests
foreach files_list : lib_gstc_client
  fname = files_list[0]
  test_name = fname.split('.')[0].underscorify()
  test_flags = gst_c_args + ['-Dmalloc=mock_malloc']

  exe = executable(test_name, files_list,
      c_args : test_flags,
      cpp_args : test_flags,
      include_directories : [configinc, lib_gstc_inc_dir],
      dependencies : [test_libgstc_deps],
  )

  # Define enviroment variable
  env.set('GST_REGISTRY', '@0@/@1@.registry'.format(meson.current_build_dir(), test_name))

  # Run tests
  test(test_name, exe, env: env, timeout : 60)

endforeach
