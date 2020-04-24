if WITH_SYSTEMD
#Install service
systemd_servicedir = @gstdsystemddir@
systemd_service_DATA = gstd.service

#Install init script
systemd_initdir = @prefix@/libexec/gstd/
systemd_init_SCRIPTS = gstd

crossbuild = '0'
if CROSSCOMPILE
	crossbuild = '1'
endif

install-data-hook:
	./gstd.service_enable.sh ${systemd_servicedir} ${crossbuild}

endif

if WITH_INITD
#Install the script
initd_scriptdir = @gstdinitddir@
initd_script_SCRIPTS = gstd

install-data-hook:
	./gstd.init_enable.sh ${initd_scriptdir}
endif
