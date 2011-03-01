#!/bin/sh

ls makefiles/*/*.mk
find . -name '*.[ch]' -type f -print >/tmp/$$
find . -name '*.[ch]pp' -type f -print >>/tmp/$$
find . -name 'GNUmakefile' -type f -print >>/tmp/$$
find . -name 'Makefile.W32' -type f -print >>/tmp/$$
find . -name '*.conf' -type f -print >>/tmp/$$

cat /tmp/$$ | sed -e 's|^\./||' | sort -u
