AC_INIT([VidRot], [0.1], [davidk@openismus.com], [vidrot])
# AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2 no-define])

AC_PROG_CXX
AC_PROG_INSTALL

GETTEXT_PACKAGE=$PACKAGE_NAME
IT_PROG_INTLTOOL([0.35])
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [The domain to use with gettext.])
AM_GLIB_GNU_GETTEXT

# VidRot needs gst-plugins-bad for the mpeg2enc element, and gst-plugins-ugly
# for the lame element.
# They are not required at build time but are necessary at run time.
# Also, gst-plugins-bad must have been built with libmjpegtools-dev (on
# Debian/Ubuntu) available.

PKG_CHECK_MODULES([VIDROT], [gtkmm-2.4 >= 2.12 gstreamermm-0.10 >= 0.10.2 gstreamer-pbutils-0.10])
AC_SUBST([VIDROT_CFLAGS])
AC_SUBST([VIDROT_LIBS])

AC_LANG([C++])

DK_ARG_ENABLE_WARNINGS([VIDROT_WARNINGS],
        [-Wall],
        [-Wall -Wextra -DGDK_DISABLE_DEPRECATED=1 -DGDK_PIXBUF_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 -DGLIBMM_DISABLE_DEPRECATED=1 -DGTK_DISABLE_DEPRECATED=1 -DGTKMM_DISABLE_DEPRECATED=1])

AC_CONFIG_FILES([
  Makefile
  vidrot.desktop.in
  po/Makefile.in
])
AC_OUTPUT
