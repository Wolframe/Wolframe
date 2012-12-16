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
	--stringparam toc.section.depth 3 \
	output-html.xsl

XSLT_PDF = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 3 \
	output-pdf.xsl

XSLT_EPUB = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 3 \
	output-epub.xsl

local_all:

doc: doc-doxygen doc-html doc-html-chunked doc-pdf

man: wolframed.8 wolframe.conf.5

doc-doxygen:
	cd $(TOPDIR); doxygen docs/doxygen.conf

doc-html: WolfBook.html

doc-html-chunked: html/index.html

doc-pdf: WolfBook.pdf

.PHONY: depend
depend:

doc-epub: WolfBook.epub
	mkdir OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  WolfBook.epub mimetype
	zip -Xr9D WolfBook.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS


%.pdf: %.fop
	fop -fo $< -pdf $@

%.fop: %.xml
	$(XSLT_PDF) $< > $@

%.html: %.xml
	$(XSLT_HTML) $< > $@

html/index.html: WolfBook.xml
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 3 \
	-o $@ output-html-chunked.xsl WolfBook.xml

%.epub: %.xml
	$(XSLT_EPUB) $<

wolframed.8: wolframed-man.xml
	$(XSLT_MAN) $<

wolframe.conf.5: wolframe.conf-man.xml
	$(XSLT_MAN) $<

local_clean:
	-rm -rf doxygen/* WolfBook.html WolfBook.fop WolfBook.pdf WolfBook.epub
	-rm -rf html/*

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/doc/wolframe
	cp -R doxygen/html/ $(DESTDIR)$(datadir)/doc/wolframe

local_uninstall:
	@-rm -rf $(DESTDIR)$(datadir)/doc/wolframe/html
	@-rmdir $(DESTDIR)$(datadir)/doc/wolframe
	@-rmdir $(DESTDIR)$(datadir)/doc
	@-rmdir $(DESTDIR)$(datadir)

check:
	xmllint --noout --xinclude --postvalid WolfBook.xml
	xmllint --noout --xinclude --postvalid wolframe.conf-man.xml
	xmllint --noout --xinclude --postvalid wolframed-man.xml
