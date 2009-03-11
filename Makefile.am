AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = src/vidrot
src_vidrot_SOURCES = src/vidrot.cpp src/main_window.cpp src/main_window.h 
src_vidrot_LDADD = $(VIDROT_LIBS)

AM_CPPFLAGS = $(VIDROT_CFLAGS) $(VIDROT_WARNINGS) $(MY_CFLAGS)
DISTCHECK_CONFIGURE_FLAGS = --enable-debug
