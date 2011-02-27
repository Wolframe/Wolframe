#!/bin/sh

ls makefiles/*/*.mk
find . -name '*.[ch]' -type f -print
find . -name '*.[ch]pp' -type f -print
find . -name 'GNUmakefile' -type f -print
find . -name 'Makefile.W32' -type f -print

# cat Wolframe.files | sed -e 's|^\./||' | sort -u
