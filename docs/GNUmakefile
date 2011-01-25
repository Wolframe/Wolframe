TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

doc: smerpd.8 smerp.conf.5
	cd $(TOPDIR); doxygen docs/doxygen.conf

XSLT_VERSION = \
	$(shell pacman -Q | grep docbook-xsl | cut -f 2 -d ' ' | cut -f 1 -d -)

XSLT_MAN_PARAMS = \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1"

XSLT_MAN = \
	$(XSLTPROC) --nonet \
	$(XSLT_MAN_PARAMS) \
	/usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
# Archlinux
#	/usr/share/xml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
# TODO: porting and probing needed here

smerpd.8: smerpd-man.xml
	$(XSLT_MAN) $<

smerp.conf.5: smerp.conf-man.xml
	$(XSLT_MAN) $<

local_clean:
	-@rm -rf doxygen 2>/dev/null
	-@rm smerpd.8 smerp.conf.5 2>/dev/null

local_all:

local_distclean:

local_test:
