AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal
SUBDIRS = po

global_defs = -DLOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

bin_PROGRAMS = src/vidrot
src_vidrot_SOURCES = src/vidrot.cc src/main_window.cc src/main_window.h
src_vidrot_LDADD = $(VIDROT_LIBS)

AM_CPPFLAGS = $(global_defs) $(VIDROT_CFLAGS) $(VIDROT_WARNINGS) $(MY_CFLAGS)

EXTRA_DIST = build-aux/config.rpath \
             vidrot.doap
