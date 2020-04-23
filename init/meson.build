cdata = configuration_data()

cdata.set('prefix', get_option('prefix'))
cdata.set('gstdrunstatedir', get_option('with-gstd-runstatedir'))
cdata.set('gstdlogstatedir', get_option('with-gstd-logstatedir'))

systemd_enabled = get_option('enable-systemd')
initd_enabled = get_option('enable-initd')

# Generate the init script
if systemd_enabled or initd_enabled
   init_file = configure_file(
   input: 'gstd.in',
   output: 'gstd',
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
   if systemd_dir == '${sysconfdir}/systemd/system'
      systemd_dir = get_option('sysconfdir') + '/systemd/system'
   endif
   install_data(service_file,
   install_dir: systemd_dir)

   meson.add_install_script('gstd.service_enable.sh', systemd_dir)
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
endif