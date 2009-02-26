AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = src/vidrot
src_vidrot_SOURCES = src/vidrot.cpp src/main_window.cpp src/main_window.h 
src_vidrot_LDADD = $(GTKMM_LIBS)

#dist_pkgdata_DATA = data/main_window_uimanager.ui

AM_CPPFLAGS = $(GTKMM_CFLAGS) $(MY_CFLAGS)
DISTCHECK_CONFIGURE_FLAGS = --enable-debug
