if WITH_SYSTEMD
#Install service
systemd_servicedir = @gstdsystemddir@
systemd_service_DATA = gstd.service

#Install init script
systemd_initdir = @prefix@/libexec/gstd/
systemd_init_SCRIPTS = gstd
endif

if WITH_INITD
#Install the script
initd_scriptdir = @gstdinitddir@
initd_script_SCRIPTS = gstd
endif
