#!/bin/sh

ls makefiles/*/*.mk
find . -name '*.[ch]' -type f -print >/tmp/$$
find . -name '*.[ch]pp' -type f -print >>/tmp/$$
find . -name 'GNUmakefile' -type f -print >>/tmp/$$
find . -name 'Makefile.W32' -type f -print >>/tmp/$$
find src/modules -name '*.def' -type f -print >>/tmp/$$
find . -name '*.conf' -type f -print >>/tmp/$$
find . -name '*.xml' -type f -print >>/tmp/$$
find . -name '*.lua' -type f -print >>/tmp/$$
find . -name '*.rng' -type f -print >>/tmp/$$
find docs -name '*.ent' -type f -print >>/tmp/$$
find . -name 'header.txt' -type f -print >>/tmp/$$
find . -name 'README*' -type f -print >>/tmp/$$
find . -name 'INSTALL*' -type f -print >>/tmp/$$
find . -name 'TODO*' -type f -print >>/tmp/$$
find . -name 'BUGS*' -type f -print >>/tmp/$$
find . -name 'passwd' -type f -print >>/tmp/$$
find . -name 'plainPasswd' -type f -print >>/tmp/$$

cat /tmp/$$ | sed -e 's|^\./||' | sort -u
