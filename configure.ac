AC_INIT([VidRot], [0.1], [davidk@openismus.com], [vidrot])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/vidrot.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_INSTALL

GETTEXT_PACKAGE=$PACKAGE
IT_PROG_INTLTOOL([0.35])
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [The domain to use with gettext.])
AM_GLIB_GNU_GETTEXT

# Disable deprecated features in libraries.
MY_CFLAGS="-DGDK_DISABLE_DEPRECATED=1 -DGDK_PIXBUF_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 -DGLIBMM_DISABLE_DEPRECATED=1 -DGTK_DISABLE_DEPRECATED=1 -DGTKMM_DISABLE_DEPRECATED=1"
AC_SUBST(MY_CFLAGS)

# We also need gst-plugins-bad for the mpeg2enc element, and gst-plugins-ugly
# for the lame element.
# We don't need them at build time (and can't check for them in a simple way),
# but we will need them at run time.
# And gst-plugins-bad must have been built with libmjpegtools-dev (on Debian/Ubuntu) available.
VIDROT_MODULES="gtkmm-2.4 >= 2.12 gstreamermm-0.10"
PKG_CHECK_MODULES(VIDROT, $VIDROT_MODULES)
AC_SUBST(VIDROT_CFLAGS)
AC_SUBST(VIDROT_LIBS)

AC_LANG([C++])

DK_ARG_ENABLE_WARNINGS([VIDROT_WARNINGS],
        [-Wall],
        [-pedantic -Wall -Wextra])

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
