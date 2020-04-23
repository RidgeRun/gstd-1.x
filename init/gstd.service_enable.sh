#!/bin/sh
# This script is executed at install time to enable
# the gstd.service on boot and start GstD
#
# $1 the gstd.service file location
# $2 whether the build is cross-compile or not

if [ $2 == 0 ]; then
    # Not cross-compilation
    if [ -x "$(command -v systemctl)" ]; then
        systemctl enable $1/gstd.service
        systemctl start gstd.service
        exit 0
    else
        echo "GstD service install error: systemctl not found" 1>&2;
        exit 1
    fi
else
    # Cross-compilation: systemctl not available at install time
    service_dir="/etc/systemd/system/multi-user.target.wants";
    mkdir -p "$service_dir"
    ln -sf $1/gstd.service "$service_dir"/gstd.service
fi
