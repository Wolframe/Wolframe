TOPDIR = ..

SUBDIRS = SDK

-include $(TOPDIR)/makefiles/gmake/doc.mk

XSLT_MAN_PARAMS = \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1"

XSLT_MAN = \
	$(XSLTPROC) --nonet \
	$(XSLT_MAN_PARAMS) \
	$(XSLT_MAN_STYLESHEET)

XSLT_HTML = \
	$(XSLTPROC) --nonet --xinclude \
	$(XSLT_HTML_STYLESHEET)

local_all:

doc-man: wolframed.8 wolframe.conf.5

doc-doxygen:
	cd $(TOPDIR); doxygen docs/doxygen.conf
	
doc-html: WolfBook.html
# xmllint --noout --xinclude --postvalid --noent WolfBook.xml
#
#	@$(XSLTPROC) --xinclude --stringparam draft.mode yes \
#		-o output/docRepo-pdf.fo eurospider-pdf.xsl Book.xml
#	DESTDIR=/ fop -fo output/docRepo-pdf.fo -pdf output/docRepo.pdf
#	rm output/docRepo-pdf.fo

WolfBook.html: WolfBook.xml
	$(XSLT_HTML) $< >$@

wolframed.8: wolframed-man.xml
	$(XSLT_MAN) $<

wolframe.conf.5: wolframe.conf-man.xml
	$(XSLT_MAN) $<

local_all:

local_clean:
	-rm -rf doxygen/* WolfBook.html

local_distclean:

local_test:
	xmllint --noout --xinclude --postvalid --noent WolfBook.xml
