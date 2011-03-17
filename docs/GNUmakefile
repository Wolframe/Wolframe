TOPDIR = ..

SUBDIRS = images

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
	--stringparam draft.mode yes \
	output-html.xsl

XSLT_PDF = \
	$(XSLTPROC) --nonet --xinclude \
	output-pdf.xsl

local_all:

doc-man: wolframed.8 wolframe.conf.5

doc-doxygen:
	cd $(TOPDIR); doxygen docs/doxygen.conf
	
doc-html: WolfBook.html

doc-pdf: WolfBook.pdf

%.pdf: %.fop
	fop -fo $< -pdf $@

%.fop: %.xml
	$(XSLT_PDF) $< > $@

%.html: %.xml
	$(XSLT_HTML) $< >$@

wolframed.8: wolframed-man.xml
	$(XSLT_MAN) $<

wolframe.conf.5: wolframe.conf-man.xml
	$(XSLT_MAN) $<

local_all:

local_clean:
	-rm -rf doxygen/* WolfBook.html WolfBook.fop WolfBook.pdf

local_distclean:

local_test:
	xmllint --noout --xinclude --postvalid --noent WolfBook.xml
