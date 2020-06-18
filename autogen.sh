#!/bin/sh
##
## GStreamer Daemon - Gst Launch under steroids
## Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
## 
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
##
# you can either set the environment variables AUTOCONF, AUTOHEADER, AUTOMAKE,
# ACLOCAL, AUTOPOINT and/or LIBTOOLIZE to the right versions, or leave them
# unset and get the defaults

gtkdocize || exit 1

autoreconf --verbose --force --install || {
 echo 'autogen.sh failed';
 exit 1;
}

# install pre-commit hook for doing clean commits
rm -f .git/hooks/pre-commit
if ! ln -s ../../common/hooks/pre-commit.hook .git/hooks/pre-commit 2> /dev/null
then
  echo "Failed to create commit hook symlink"
fi

echo
echo "Now run './configure' with your system settings followed by 'make' to compile this module."
echo
