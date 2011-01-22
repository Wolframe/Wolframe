TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

doc:
	cd $(TOPDIR); doxygen docs/doxygen.conf

local_clean:
	-@rm -rf doxygen 2>/dev/null

local_all:

local_distclean:

local_test:

# xsltproc --nonet --param man.charmap.use.subset "0" --param make.year.ranges "1" --param make.single.year.ranges "1" /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl manpage.xml
