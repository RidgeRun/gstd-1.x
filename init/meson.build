cdata = configuration_data()
cdata.set('GSTD_RUN_STATE_DIR', runstatedir)
cdata.set('GSTD_LOG_STATE_DIR', logstatedir)
cdata.set('prefix', prefix)
cdata.set('GSTD_STATE_DIRS_OWNER', get_option('with-statedir-owner'))

initd_enabled = get_option('enable-initd').enabled()
systemd_enabled = not get_option('enable-systemd').disabled()

if systemd_enabled and systemd_dep.found()
   # Generate service file
   service_file = configure_file(
   input: 'gstd.service.in',
   output: 'gstd.service',
   configuration: cdata)

   # Install service file
   systemd_dir = get_option('with-gstd-systemddir')
   if systemd_dir == 'Systemd default systemduserunitdir'
      systemd_dir = systemd_dep.get_pkgconfig_variable( \
      'systemduserunitdir')
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
