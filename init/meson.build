cdata = configuration_data()

prefix = get_option('prefix')
runstatedir = get_option('with-gstd-runstatedir')
logstatedir = get_option('with-gstd-logstatedir')

# Replace the ${prefix} by the actual prefix value
if runstatedir.contains('${prefix}')
   runstatedir = prefix + runstatedir.split('${prefix}')[1]
endif
if logstatedir.contains('${prefix}')
   logstatedir = prefix + logstatedir.split('${prefix}')[1]
endif

cdata.set('prefix', prefix)
cdata.set('gstdrunstatedir', runstatedir)
cdata.set('gstdlogstatedir', logstatedir)
cdata.set('initusername', get_option('with-init-username'))

initd_enabled = false
if get_option('with-init-username') != 'none'
   initd_enabled = get_option('enable-initd')
endif

systemd_enabled = get_option('enable-systemd')

# Generate the init script
if systemd_enabled
   initd_dir = get_option('with-gstd-initddir')
   if initd_dir == '${sysconfdir}/init.d'
      initd_dir = get_option('sysconfdir') + '/init.d'
   endif
   init_file = configure_file(
   input: 'gstd.conf.in',
   output: 'gstd.conf',
   configuration: cdata)
endif

if systemd_enabled
   # Install init script
   install_data(init_file,
   install_dir: get_option('prefix') + '/libexec/gstd',
   install_mode: 'r-xr-xr-x')

   # Generate service file
   service_file = configure_file(
   input: 'gstd.service.in',
   output: 'gstd.service',
   configuration: cdata)

   # Install service file
   systemd_dir = get_option('with-gstd-systemddir')
   if systemd_dir == '${sysconfdir}/systemd/user'
      systemd_dir = get_option('sysconfdir') + '/systemd/user'
   endif
   install_data(service_file,
   install_dir: systemd_dir)
endif

if initd_enabled
   # Install init script
   initd_dir = get_option('with-gstd-initddir')
   if initd_dir == '${sysconfdir}/init.d'
      initd_dir = get_option('sysconfdir') + '/init.d'
   endif
   install_data(init_file,
   install_dir: initd_dir,
   install_mode: 'r-xr-xr-x')

   meson.add_install_script('gstd.init_enable.sh', initd_dir)
endif