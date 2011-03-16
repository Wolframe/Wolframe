TOPDIR = ..

SUBDIRS = SDK

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

doc: wolframed.8 wolframe.conf.5
	cd $(TOPDIR); doxygen docs/doxygen.conf

# xmllint --noout --xinclude --postvalid --noent WolfBook.xml
#
#	@$(XSLTPROC) --xinclude --stringparam draft.mode yes \
#		-o output/docRepo-pdf.fo eurospider-pdf.xsl Book.xml
#	DESTDIR=/ fop -fo output/docRepo-pdf.fo -pdf output/docRepo.pdf
#	rm output/docRepo-pdf.fo

XSLT_MAN_PARAMS = \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1"

XSLT_MAN = \
	$(XSLTPROC) --nonet \
	$(XSLT_MAN_PARAMS) \
	$(XSLT_MAN_STYLESHEET)

wolframed.8: wolframed-man.xml
	$(XSLT_MAN) $<

wolframe.conf.5: wolframe.conf-man.xml
	$(XSLT_MAN) $<

local_clean:

local_distclean:
	-rm -rf doxygen/*

local_test:
