#!/bin/sh
# This script is executed at install time to enable the gstd.service on boot
# and start GstD
# $1 the gstd.service file location

systemctl enable $1/gstd.service
systemctl start gstd.service
