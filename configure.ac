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

AC_MSG_CHECKING(whether to build with debugging information)
debugger=no
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging symbols and messages (default=no)]),debugger="$enableval")
AC_MSG_RESULT($debugger)
if test x"$debugger" = x"yes"; then
	AC_DEFINE(DEBUG, 1, [Define to enable debugging symbols and messages])
	DEBUG_CFLAGS="-g -Werror"
	AC_SUBST([CFLAGS], $DEBUG_CFLAGS)
fi

# Disable depracated features in libraries.
MY_CFLAGS="-DGDK_DISABLE_DEPRECATED=1 -DGDK_PIXBUF_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 -DGLIBMM_DISABLE_DEPRECATED=1 -DGTK_DISABLE_DEPRECATED=1 -DGTKMM_DISABLE_DEPRECATED=1"
AC_SUBST(MY_CFLAGS)

VIDROT_MODULES="gtkmm-2.4 >= 2.12 gstreamermm-0.10"
PKG_CHECK_MODULES(VIDROT, $VIDROT_MODULES)
AC_SUBST(VIDROT_CFLAGS)
AC_SUBST(VIDROT_LIBS)

AC_LANG([C++])

DK_ARG_ENABLE_WARNINGS([VIDROT_WARNINGS],
        [-Wall],
        [-pedantic -Wall -Wextra])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
