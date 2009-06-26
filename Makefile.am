AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal
SUBDIRS = po

global_defs = -DLOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

bin_PROGRAMS = src/vidrot
src_vidrot_SOURCES = src/main.cc src/main_window.cc src/main_window.h src/vidrot_preview.cc src/vidrot_preview.h
src_vidrot_LDADD = $(VIDROT_LIBS)

AM_CPPFLAGS = $(global_defs) $(VIDROT_CFLAGS) $(VIDROT_WARNINGS) $(MY_CFLAGS)


#Copied from gnome-terminal's Makefile.am:
desktopdir = $(datadir)/applications
desktop_in_files = vidrot.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = data/vidrot_logo.png \
             vidrot.doap
