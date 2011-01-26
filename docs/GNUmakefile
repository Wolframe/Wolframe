TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

doc: smerpd.8 smerp.conf.5
	cd $(TOPDIR); doxygen docs/doxygen.conf

XSLT_MAN_PARAMS = \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1"

XSLT_MAN = \
	$(XSLTPROC) --nonet \
	$(XSLT_MAN_PARAMS) \
	$(XSLT_MAN_STYLESHEET)

smerpd.8: smerpd-man.xml
	$(XSLT_MAN) $<

smerp.conf.5: smerp.conf-man.xml
	$(XSLT_MAN) $<

local_clean:

local_distclean:

local_test:
