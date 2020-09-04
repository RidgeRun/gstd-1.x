if WITH_SYSTEMD
#Install service
systemd_servicedir = @GSTD_SYSTEMD_DIR@
systemd_service_DATA = gstd.service
systemd_service_SCRIPTS = gstd-check-user-xenv.sh
endif

if WITH_INITD
#Install the script
initd_scriptdir = @GSTD_INITD_DIR@
initd_script_SCRIPTS = gstd
endif
