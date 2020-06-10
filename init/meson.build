cdata = configuration_data()
cdata.set('gstdrunstatedir', runstatedir)
cdata.set('gstdlogstatedir', logstatedir)
cdata.set('prefix', prefix)
cdata.set('owner', get_option('with-statedir-owner'))

initd_enabled = get_option('enable-initd')
systemd_enabled = get_option('enable-systemd')

# Generate the init script
if systemd_enabled
   initd_dir = get_option('with-gstd-initddir')
   if initd_dir == '${sysconfdir}/init.d'
      initd_dir = get_option('sysconfdir') + '/init.d'
   endif

   # Generate tmpfiles.d config file
   init_file = configure_file(
   input: 'gstd.conf.in',
   output: 'gstd.conf',
   configuration: cdata)

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
   # Generate init script
   init_file = configure_file(
   input: 'gstd.in',
   output: 'gstd',
   configuration: cdata)

   # Install init script
   initd_dir = get_option('with-gstd-initddir')
   if initd_dir == '${sysconfdir}/init.d'
      initd_dir = get_option('sysconfdir') + '/init.d'
   endif
   install_data(init_file,
   install_dir: initd_dir,
   install_mode: 'r-xr-xr-x')
endif