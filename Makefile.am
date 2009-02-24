AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = src/vidrot
src_vidrot_SOURCES = src/vidrot.c
src_vidrot_LDADD = $(GTK_LIBS)

#dist_pkgdata_DATA = data/main_window_uimanager.ui

AM_CPPFLAGS = $(GTK_CFLAGS) $(MY_CFLAGS)
DISTCHECK_CONFIGURE_FLAGS = --enable-debug
