cdata = configuration_data()

cdata.set('prefix', get_option('prefix'))
cdata.set('gstdrunstatedir', get_option('with-gstd-runstatedir'))
cdata.set('gstdlogstatedir', get_option('with-gstd-logstatedir'))

systemd_dir = get_option('with-systemd')

if systemd_dir != 'no'

   # Generate init files
   init_file = configure_file(
   input: 'gstd.in',
   output: 'gstd',
   configuration: cdata
   )

   service_file = configure_file(
   input: 'gstd.service.in',
   output: 'gstd.service',
   configuration: cdata
   )

   # Install init script
   install_data(init_file,
   install_dir: get_option('prefix') + '/libexec/gstd',
   install_mode: 'r-xr-xr-x'
   )

   # Install service file
   if systemd_dir == 'yes' or systemd_dir == '${sysconfdir}/systemd/system'
      systemd_dir = get_option('sysconfdir') + '/systemd/system'
   endif
   install_data(service_file,
   install_dir: systemd_dir
   )
endif