#!/bin/sh

ls makefiles/*/*.mk
find . -name '*.[ch]' -type f -print >/tmp/$$
find . -name '*.[ch]pp' -type f -print >>/tmp/$$
find . -name 'GNUmakefile' -type f -print >>/tmp/$$
find . -name 'Makefile.W32' -type f -print >>/tmp/$$
find . -name '*.conf' -type f -print >>/tmp/$$
find . -name '*.xml' -type f -print >>/tmp/$$
find . -name '*.rng' -type f -print >>/tmp/$$
find docs -name '*.ent' -type f -print >>/tmp/$$
find . -name 'header.txt' -type f -print >>/tmp/$$

cat /tmp/$$ | sed -e 's|^\./||' | sort -u
