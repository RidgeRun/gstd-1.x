project('gstd', 'c',
  version : '0.7.0',
  meson_version : '>= 0.50',)

gstd_version = meson.project_version()
version_arr = gstd_version.split('.')
gstd_version_major = version_arr[0].to_int()
gstd_version_minor = version_arr[1].to_int()
gstd_version_micro = version_arr[2].to_int()
if version_arr.length() == 4
  gstd_version_nano = version_arr[3].to_int()
else
  gstd_version_nano = 0
endif
gstd_version_is_dev = gstd_version_minor % 2 == 1 and gstd_version_micro < 90


# Find external dependencies
gst_dep       = dependency('gstreamer-1.0',      version : '>=1.0.0')
gst_base_dep  = dependency('gstreamer-base-1.0', version : '>=1.0.0')
gio_unix_dep  = dependency('gio-unix-2.0',       version : '>=2.44.1')
json_glib_dep = dependency('json-glib-1.0',      version : '>=0.16.2')
libd_dep      = dependency('libdaemon',          version : '>=0.14')
jansson_dep   = dependency('jansson',            version : '>=2.7')
gst_check_dep = dependency('gstreamer-check-1.0',version : '>=1.0.5')
thread_dep    = dependency('threads')


cc = meson.get_compiler('c')
readline = cc.find_library('readline', required: true)
if readline.found()
  add_project_arguments('-DHAVE_LIBREADLINE', language: 'c')
  add_project_arguments('-DHAVE_READLINE_HISTORY', language: 'c')
  # Add arguments to the compiler command line. 
  if cc.has_header('readline/readline.h')
    add_project_arguments('-DHAVE_READLINE_READLINE_H', language: 'c')
  else
    add_project_arguments('-DHAVE_READLINE_H', language: 'c')
  endif
  
  if cc.has_header('readline/history.h')
    add_project_arguments('-DHAVE_READLINE_HISTORY_H', language: 'c')
  else
    add_project_arguments('-DHAVE_HISTORY_H', language: 'c')
  endif
  
  readline_dep = readline
endif

## Dependencies
# Define gst Daemon dependencies
gstd_deps = [gst_base_dep, gio_unix_dep, json_glib_dep, libd_dep, jansson_dep]
# Define gst client library dependencies
libgstc_deps = [gst_base_dep, json_glib_dep, jansson_dep, thread_dep]
# Define gst client application dependencies
gst_client_deps = [readline_dep, json_glib_dep, gio_unix_dep]
# Define test dependencies
test_libgstc_deps=[gst_base_dep, json_glib_dep, jansson_dep, thread_dep, gst_check_dep]
test_gstd_deps=[gst_base_dep, gio_unix_dep, json_glib_dep, libd_dep, jansson_dep, gst_check_dep]

# Define header directories
lib_gstc_inc_dir = include_directories('libgstc')
gstd_inc_dir = include_directories('gstd')
configinc = include_directories('.')

# Define compiler args and include directories
gstd_run_state_dir = '-DGSTD_RUN_STATE_DIR="@0@"'.format(get_option('prefix') + get_option('with-gstd-runstatedir'))
gstd_log_state_dir = '-DGSTD_LOG_STATE_DIR="@0@"'.format(get_option('prefix') + get_option('with-gstd-logstatedir'))
gst_c_args = ['-DHAVE_CONFIG_H',gstd_log_state_dir,gstd_run_state_dir]

# Define gstreamer API version
apiversion = '1.0'

# Define installation directories
prefix = get_option('prefix')
lib_install_dir = get_option('libdir')
# Internal installation directory
lib_gstd_dir = []
lib_gstc_dir = []


# Get an object returns describing a compiler
cc = meson.get_compiler('c')

# Verify if the warning flags are available in the compiler
# If the flags is availale for the compiler it wiil be used in all compiler
# invocations with the exception of compile tests.
warning_flags = [
  '-Wmissing-declarations',
  '-Wmissing-prototypes',
  '-Wredundant-decls',
  '-Wundef',
  '-Wwrite-strings',
  '-Wformat',
  '-Wformat-nonliteral',
  '-Wformat-security',
  '-Wold-style-definition',
  '-Winit-self',
  '-Wmissing-include-dirs',
  '-Waddress',
  '-Waggregate-return',
  '-Wno-multichar',
  '-Wdeclaration-after-statement',
  '-Wvla',
  '-Wpointer-arith',
]
foreach extra_arg : warning_flags
  if cc.has_argument (extra_arg)
    # Add flag to the compiler command line
    add_project_arguments([extra_arg], language: 'c')
  endif
endforeach

# Create an empty configuration object to set config.h information
cdata = configuration_data()

pkg_gstd_url='https://developer.ridgerun.com/wiki/index.php?title=GStreamer_Daemon'
# Set config.h information
cdata.set_quoted('GST_API_VERSION', apiversion)
cdata.set_quoted('LOCALEDIR', join_paths(prefix, get_option('localedir')))
cdata.set_quoted('LIBDIR', join_paths(prefix, get_option('libdir')))
cdata.set_quoted('GST_API_VERSION', '1.0')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('PACKAGE', 'gstd')
cdata.set_quoted('PACKAGE_NAME', 'gstd')
cdata.set_quoted('PACKAGE_STRING', 'gstd @0@'.format(gstd_version))
cdata.set_quoted('PACKAGE_TARNAME', 'gstd')
cdata.set_quoted('PACKAGE_BUGREPORT', ' https://github.com/RidgeRun/gstd-1.x.git')
cdata.set_quoted('PACKAGE_URL', pkg_gstd_url)
cdata.set_quoted('PACKAGE_VERSION', gstd_version)
cdata.set_quoted('PLUGINDIR', lib_install_dir)
cdata.set_quoted('VERSION', gstd_version)

if gstd_version_nano > 0
    # Have GST_ERROR message printed when running from git
    cdata.set('GST_LEVEL_DEFAULT', 'GST_LEVEL_ERROR')
else
    cdata.set('GST_LEVEL_DEFAULT', 'GST_LEVEL_NONE')
endif

# GStreamer package name and origin url
gst_package_name = get_option('package-name')
if gst_package_name == ''
  if gstd_version_nano == 0
    gst_package_name = 'Gstd source release'
  elif gstd_version_nano == 1
    gst_package_name = 'GStreamer git'
  else
    gst_package_name = 'GStreamer prerelease'
  endif
endif
cdata.set_quoted('GST_PACKAGE_NAME', gst_package_name)
cdata.set_quoted('GST_PACKAGE_ORIGIN', get_option('package-origin'))

# These are only needed/used by the ABI tests
host_defines = [
  [ 'x86', 'HAVE_CPU_I386' ],
  [ 'x86_64', 'HAVE_CPU_X86_64' ],
  [ 'arm', 'HAVE_CPU_ARM' ],
  [ 'aarch64', 'HAVE_CPU_AARCH64' ],
  [ 'mips', 'HAVE_CPU_MIPS' ],
  [ 'powerpc', 'HAVE_CPU_PPC' ],
  [ 'powerpc64', 'HAVE_CPU_PPC64' ],
  [ 'alpha', 'HAVE_CPU_ALPHA' ],
  [ 'sparc', 'HAVE_CPU_SPARC' ],
  [ 'ia64', 'HAVE_CPU_IA64' ],
  [ 'hppa', 'HAVE_CPU_HPPA' ],
  [ 'm68k', 'HAVE_CPU_M68K' ],
  [ 's390', 'HAVE_CPU_S390' ],
]
foreach h : host_defines
  if h.get(0) == host_machine.cpu()
    cdata.set(h.get(1), 1)
  else
    cdata.set(h.get(1), false)
  endif
endforeach

cdata.set_quoted('HOST_CPU', host_machine.cpu())

# Verify if the specified header exists
check_headers = [
  'dlfcn.h',
  'inttypes.h',
  'memory.h',
  'poll.h',
  'stdint.h',
  'stdlib.h',
  'stdio_ext.h',
  'strings.h',
  'string.h',
  'sys/param.h',
  'sys/poll.h',
  'sys/prctl.h',
  'sys/socket.h',
  'sys/stat.h',
  'sys/times.h',
  'sys/time.h',
  'sys/types.h',
  'sys/utsname.h',
  'sys/wait.h',
  'ucontext.h',
  'unistd.h',
  'valgrind/valgrind.h',
  'sys/resource.h',
]

foreach h : check_headers
  if cc.has_header(h)
    define = 'HAVE_' + h.underscorify().to_upper()
    cdata.set(define, 1)
  endif
endforeach

# Gtk documentation
gnome = import('gnome')
# Imports pkgconfig module
pkgconfig = import('pkgconfig')

# Meson will generate a header file all the entries in the configuration data object
configure_file(output : 'config.h', configuration : cdata)

# Enter to each subdirectory and execute the meson.build
subdir('gstd')
subdir('libgstc')
subdir('gst_client')
subdir('tests')
subdir('examples')
subdir('docs')
subdir('init')