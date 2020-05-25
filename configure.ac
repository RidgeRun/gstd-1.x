dnl required version of autoconf
AC_PREREQ([2.53])

dnl Gstreamer's daemon package name and version
AC_INIT([gstd],[0.10.0])

dnl required version of gstreamer and gst-plugins-base
GST_REQUIRED=1.0.0

dnl required version of gio
GIO_REQUIRED=2.44.1

dnl required version of gio-unix
GIO_UNIX_REQUIRED=2.44.1

dnl required version of gjson
GJSON_REQUIRED=0.16.2

dnl required version of libdaemon
LIBD_REQUIRED=0.14

dnl required version of jansson
JANSSON_REQUIRED=2.7

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

GSTD_API_VERSION=1.0
AC_SUBST(GSTD_API_VERSION)

dnl error out if we can't find pkg-config
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed or set the PATH.])
])

PKG_CHECK_MODULES(GST, [
    gstreamer-1.0              >= $GST_REQUIRED
    gstreamer-base-1.0         >= $GST_REQUIRED
    gstreamer-check-1.0        >= $GST_REQUIRED
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

PKG_CHECK_MODULES(GIO_UNIX, [
    gio-unix-2.0              >= $GIO_UNIX_REQUIRED
  ], [
    AC_SUBST(GIO_UNIX_CFLAGS)
    AC_SUBST(GIO_UNIX_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GIO-UNIX development packages:

      gio-unix-2.0              >= $GIO_UNIX_REQUIRED

    Please make sure you have the necessary GIO-UNIX-2.0
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libglib2.0-dev' package.

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
if test "x$ax_cv_lib_readline" = "xno"; then
  AC_MSG_ERROR([
       Please install readline development packages. In Debian based
       systems you may run:
         - readline : "sudo apt-get install libreadline-dev"

       Additionally, install one of the following:
        - ncurses  : "sudo apt-get install libncurses5-dev"
        - ncurserw : "sudo apt-get install libncursesw5-dev"
  ])
else
  AC_SUBST(READLINE_CFLAGS, )
  AC_SUBST(READLINE_LIBS, $ax_cv_lib_readline)
fi

PKG_CHECK_MODULES(LIBD, [
    libdaemon >= $LIBD_REQUIRED
  ], [
    AC_SUBST(LIBD_CFLAGS)
    AC_SUBST(LIBD_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find libdaemon development packages:

      libdaemon-dev >= $LIBD_REQUIRED

    Please make sure you have the necessary libdaemon-dev
    development headers installed.
  ])
])

PKG_CHECK_MODULES(JANSSON, [
    jansson >= $JANSSON_REQUIRED
  ], [
    AC_SUBST(JANSSON_CFLAGS)
    AC_SUBST(JANSSON_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find janssonaemon development packages:

      libjansson-dev >= $JANSSON_REQUIRED

    Please make sure you have the necessary libjansson-dev
    development headers installed.
  ])
])

dnl allow the user to specify the location of the PID files
default=${localstatedir}/run/gstd/
AC_ARG_WITH([gstd-runstatedir],
  [AS_HELP_STRING([--with-gstd-runstatedir],
    [Specify the location of the gstd's PID file @<:@default=${default}@:>@])],
  [AC_SUBST(gstdrunstatedir, [$with_gstd_runstatedir])],
  [AC_SUBST(gstdrunstatedir, [$default])])

dnl allow the user to specify the location of the log files
default=${localstatedir}/log/gstd/
AC_ARG_WITH([gstd-logstatedir],
  [AS_HELP_STRING([--with-gstd-logstatedir],
    [Specify the location of gstd's log files @<:@default=${default}@:>@])],
  [AC_SUBST([gstdlogstatedir], [$with_gstd_logstatedir])],
  [AC_SUBST([gstdlogstatedir], [$default])])

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
gst_client/Makefile
libgstc/Makefile
examples/Makefile
examples/libgstc/Makefile
tests/Makefile
tests/gstd/Makefile
tests/libgstc/Makefile
tests/libgstc/pygstc/Makefile
docs/Makefile
docs/reference/Makefile
docs/reference/gstd/Makefile
])
AC_OUTPUT
