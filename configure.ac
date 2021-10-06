dnl required version of autoconf
AC_PREREQ([2.53])

dnl Gstreamer's daemon package name and version
AC_INIT([gstd],[0.13.0])

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

dnl required version of systemd
SYSTEMD_REQUIRED=232

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

PKG_CHECK_MODULES(LIBSOUP, [libsoup-2.4 >= 2.32])
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)

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
  [AC_SUBST(GSTD_RUN_STATE_DIR, [$with_gstd_runstatedir])],
  [AC_SUBST(GSTD_RUN_STATE_DIR, [$default])])

dnl allow the user to specify the location of the log files
default=${localstatedir}/log/gstd/
AC_ARG_WITH([gstd-logstatedir],
  [AS_HELP_STRING([--with-gstd-logstatedir],
    [Specify the location of gstd's log files @<:@default=${default}@:>@])],
  [AC_SUBST([GSTD_LOG_STATE_DIR], [$with_gstd_logstatedir/])],
  [AC_SUBST([GSTD_LOG_STATE_DIR], [$default])])

dnl allow the user to disable systemd gstd.service install
systemd=true
systemd_required=true
AC_ARG_ENABLE(systemd,
  AS_HELP_STRING([--enable-systemd], [Enable/disable install of systemd gstd.service]),
  [ if test "x$enable_systemd" = "xno" ; then
      systemd=false
      systemd_required=false
    fi
  ],
  [
    systemd_required=false
  ])

dnl check for systemd dependency
if test "$systemd" = true; then
  PKG_CHECK_MODULES(SYSTEMD, [
    systemd >= $SYSTEMD_REQUIRED
    ], [
        AC_SUBST([GSTD_SYSTEMD_CONF_DIR], [$(pkg-config systemd --variable=tmpfilesdir)])
        AC_SUBST([GSTD_SYSTEMD_DIR], [$(pkg-config systemd --variable=systemduserunitdir)])
    ], [
      systemd=false
      if test "$systemd_required" = true; then
        AC_MSG_ERROR([
          Can't find systemd development packages:

          systemd >= $SYSTEMD_REQUIRED

          Please make sure you have the necessary systemd
          development headers installed.
        ])
      else
        AC_MSG_WARN([
          Can't find systemd development packages:

          gstd.service will not be installed
        ])
      fi
    ])
fi
AM_CONDITIONAL(WITH_SYSTEMD, test "$systemd" = true)

dnl allow the user to specify systemd gstd.service file location
AC_ARG_WITH([gstd-systemddir],
  AS_HELP_STRING([--with-gstd-systemddir@<:@=DIR@:>@],
  [Install systemd gstd.service @<:@to DIR if specified@:>@, otherwise defaults to systemd variable systemduserunitdir]),
  [AC_SUBST([GSTD_SYSTEMD_DIR], [$with_gstd_systemddir])], [])

dnl allow the user to enable init script install
AC_ARG_ENABLE(initd,
  AS_HELP_STRING([--enable-initd], [Enable install of init.d script]),
  [ initd=true ],
  [ initd=false ])
AM_CONDITIONAL(WITH_INITD, test "$initd" = true)

dnl allow the user to specify init script location
default=${sysconfdir}/init.d/
AC_ARG_WITH([gstd-initddir],
  AS_HELP_STRING([--with-gstd-initddir@<:@=DIR@:>@],
  [Install init script @<:@to DIR if specified@:>@, otherwise defaults to ${sysconfdir}/init.d/]),
  [AC_SUBST([GSTD_INITD_DIR], [$with_gstd_initddir])],
  [AC_SUBST([GSTD_INITD_DIR], [$default])])

dnl allow the user to specify the owner of the state dirs
AC_ARG_WITH([statedir-owner],
  AS_HELP_STRING([--with-statedir-owner@<:@=USERNAME@:>@],
  [Set the state initd created dirs ownership to @<:@USERNAME@:>@, @<:@default=root@:>@]),
  [AC_SUBST([GSTD_STATE_DIRS_OWNER], [$with_statedir_owner])],
  [AC_SUBST([GSTD_STATE_DIRS_OWNER], ["root"])])

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
libgstc/c/Makefile
libgstc/javascript/Makefile
libgstc/python/Makefile
libgstd/Makefile
examples/Makefile
examples/libgstc/Makefile
tests/Makefile
tests/gstd/Makefile
tests/libgstc/Makefile
tests/libgstc/c/Makefile
tests/libgstc/python/Makefile
tests/libgstd/Makefile
docs/Makefile
docs/reference/Makefile
docs/reference/gstd/Makefile
init/Makefile
init/gstd
init/gstd.service
])
AC_OUTPUT
