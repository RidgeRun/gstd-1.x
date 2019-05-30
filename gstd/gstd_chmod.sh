#!/bin/sh
# This script is executed at install time to change the mode of the directories installed

chmod 777 "${DESTDIR}/${MESON_INSTALL_PREFIX}/$1"
