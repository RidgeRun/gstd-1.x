#!/bin/sh
# This script is executed at install time to create and install symbolic links

# $1: target
# $2: link name
rm -f ${DESTDIR}${MESON_INSTALL_PREFIX}/$2
ln -s ./$1 ${DESTDIR}${MESON_INSTALL_PREFIX}/$2
