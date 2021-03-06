#!/bin/sh -e

glib-gettextize -c -f
intltoolize --automake --copy --force
${ACLOCAL-aclocal} -I m4 $ACLOCAL_FLAGS
${AUTOCONF-autoconf}
${AUTOHEADER-autoheader}
${AUTOMAKE-automake} --add-missing --copy $AUTOMAKE_FLAGS

# autoreconf fails to include the m4 folder. Lame.
#autoreconf --install
./configure "$@"
