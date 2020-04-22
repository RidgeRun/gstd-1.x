if WITH_SYSTEMD
#Install service
systemddir = @systemd_path@
dist_systemd_DATA = gstd.service

#Install init script
system_ddir = @prefix@/libexec/gstd/
system_d_SCRIPTS = gstd
endif

if WITH_INITD
#Install the script
init_ddir = @initd_path@
init_d_SCRIPTS = gstd
endif
