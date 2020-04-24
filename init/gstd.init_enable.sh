#!/bin/sh
# This script is executed at install time to enable
# the init script on boot
#
# $1 the gstd init file directory

ln -sf $1/gstd /etc/rc0.d/K99gstd
ln -sf $1/gstd /etc/rc6.d/K99gstd
ln -sf $1/gstd /etc/rc5.d/S99gstd
