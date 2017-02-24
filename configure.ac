dnl required version of autoconf
AC_PREREQ([2.53])

dnl Gstreamer's daemon package name and version
AC_INIT([gstd],[0.3.0])

dnl required version of gstreamer and gst-plugins-base
GST_REQUIRED=1.0.0

dnl required version of gio
GIO_REQUIRED=2.44.1

dnl required version of gio
GJSON_REQUIRED=0.16.2

AC_CONFIG_SRCDIR([gstd/gstd.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10 subdir-objects])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for tools (compiler etc.)
AM_PROG_CC_C_O

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

dnl error out if we can't find pkg-config
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed or set the PATH.])
])

PKG_CHECK_MODULES(GST, [
    gstreamer-1.0              >= $GST_REQUIRED
    gstreamer-base-1.0         >= $GST_REQUIRED
  ], [
    AC_SUBST(GST_CFLAGS)
    AC_SUBST(GST_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GStreamer development packages:

      gstreamer-1.0              >= $GST_REQUIRED
      gstreamer-base-1.0         >= $GST_REQUIRED

    Please make sure you have the necessary GStreamer-1.0
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libgstreamer1.0-dev' and 'libgstreamer-plugins-base1.0-dev' packages.

    On RPM-based systems you will probably need to install the
    'gstreamer-devel-1.0' package.
  ])
])

PKG_CHECK_MODULES(GIO, [
    gio-2.0              >= $GST_REQUIRED
  ], [
    AC_SUBST(GIO_CFLAGS)
    AC_SUBST(GIO_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GIO development packages:

      gio-2.0              >= $GIO_REQUIRED

    Please make sure you have the necessary GIO-2.0
    development headers installed.
  ])
])

PKG_CHECK_MODULES(GJSON, [
    json-glib-1.0              >= $GJSON_REQUIRED
  ], [
    AC_SUBST(GJSON_CFLAGS)
    AC_SUBST(GJSON_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GIO development packages:

      json-glib-1.0              >= $GJSON_REQUIRED

    Please make sure you have the necessary json-glib-1.0
    development headers installed.
  ])
])


dnl check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl --docdir docs])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AX_LIB_READLINE

dnl check if compiler understands -Wall (if yes, add -Wall to GST_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])
                                                                                
dnl =======================================================================
dnl Finally, create Makefiles in all directories
dnl =======================================================================

AC_CONFIG_FILES([
Makefile
gstd/Makefile
gstd_client/Makefile
tests/Makefile
tests/gstd/Makefile
docs/Makefile
docs/reference/Makefile
docs/reference/gstd/Makefile
])
AC_OUTPUT

