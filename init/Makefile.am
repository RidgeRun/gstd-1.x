if WITH_SYSTEMD
#Install service
systemd_servicedir = @GSTD_SYSTEMD_DIR@
systemd_service_DATA = gstd.service

#Install tmpfiles.d
systemd_tmpfilesdir = @GSTD_SYSTEMD_CONF_DIR@
systemd_tmpfiles_DATA = gstd.conf

#Autoconf doesn't expand the ${prefix}
install-exec-hook:
	sed -i 's|$${prefix}|${prefix}|g' gstd.conf
endif

if WITH_INITD
#Install the script
initd_scriptdir = @GSTD_INITD_DIR@
initd_script_SCRIPTS = gstd
endif
