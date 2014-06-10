#!/bin/sh

ls makefiles/*/*.mk
find . -name '*.[ch]' -type f -print >/tmp/$$
find . -name '*.[ch]pp' -type f -print >>/tmp/$$
find . -name 'GNUmakefile' -type f -print >>/tmp/$$
find . -name 'Makefile.W32' -type f -print >>/tmp/$$
find src/modules -name '*.def' -type f -print >>/tmp/$$
find . -name '*.conf' -type f -print >>/tmp/$$
find . -name '*.exp' -type f -print >>/tmp/$$
find . -name '*.xml' -type f -print >>/tmp/$$
find . -name '*.xsl' -type f -print >>/tmp/$$
find . -name '*.lua' -type f -print >>/tmp/$$
find . -name '*.rng' -type f -print >>/tmp/$$
find . -name '*.sql' -type f -print >>/tmp/$$
find . -name '*.netcat' -type f -print >>/tmp/$$
find . -name '*.simpleform' -type f -print >>/tmp/$$
find . -name '*.tdl' -type f -print >>/tmp/$$
find . -name '*.normalize' -type f -print >>/tmp/$$
find . -name '*.sh' -type f -print >>/tmp/$$
find docs -name '*.ent' -type f -print >>/tmp/$$
find tests -name '*.tst' -type f -print >>/tmp/$$
find . -name 'header.txt' -type f -print >>/tmp/$$
find . -iname 'AUTHORS*' -type f -print >>/tmp/$$
find . -iname 'Credits*' -type f -print >>/tmp/$$
find . -iname 'README*' -type f -print >>/tmp/$$
find . -iname 'PAMPHLET*' -type f -print >>/tmp/$$
find . -iname 'INSTALL*' -type f -print >>/tmp/$$
find . -iname 'TODO*' -type f -print >>/tmp/$$
find . -iname 'BUGS*' -type f -print >>/tmp/$$
find . -name 'passwd' -type f -print >>/tmp/$$
find . -name '.gitignore' -type f -print >>/tmp/$$
find . -name '.gitattributes' -type f -print >>/tmp/$$
find . -name 'create_qtcreator_files.sh' -type f -print >>/tmp/$$

cat /tmp/$$ | sed -e 's|^\./||' | sort -u
