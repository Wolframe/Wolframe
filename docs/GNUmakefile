TOPDIR = ..

SUBDIRS =

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
	--stringparam toc.section.depth 4 \
	output-html.xsl

XSLT_WEBPAGE = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam toc.section.depth 4 \
	output-html-webpage.xsl

XSLT_PDF = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	--stringparam fop1.extensions 1 \
	output-pdf.xsl

XSLT_EPUB = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	output-epub.xsl

local_all:

doc: doc-doxygen doc-html doc-html-chunked doc-pdf

man: wolframed.8 wolframe.conf.5 wolfpasswd.1

doc-doxygen:
	cd $(TOPDIR); doxygen docs/doxygen.conf

doc-doxygen-webpage:
	@-mkdir doxygen/html-webpage
	cd $(TOPDIR); doxygen docs/doxygen-webpage.conf

doc-html: WolfBook.html Tutorial.html

doc-html-chunked: html/index.html

doc-html-webpage: html-webpage/index.html

doc-pdf: WolfBook.pdf Tutorial.pdf

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

html-webpage/index.html: WolfBook.xml
	$(XSLTPROC) --nonet --xinclude \
	--stringparam toc.section.depth 3 \
	-o $@ output-html-webpage.xsl WolfBook.xml

%.epub: %.xml
	$(XSLT_EPUB) $<

wolframed.8: wolframed-man.xml
	$(XSLT_MAN) $<

wolframe.conf.5: wolframe.conf-man.xml
	$(XSLT_MAN) $<

wolfpasswd.1: wolfpasswd-man.xml
	$(XSLT_MAN) $<

Tutorial.html: Tutorial.xml

local_clean:
	-rm -rf doxygen/html/* doxygen/man/* WolfBook.html WolfBook.fop WolfBook.pdf WolfBook.epub
	-rm -f Tutorial.html Tutorial.pdf
	-rm -rf html/*.html
	-rm -rf html-webpage/*.html
	-rm -rf doxygen/html-webpage/*
	
local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/doc/wolframe
	-test -d doxygen/html && cp -R doxygen/html/ $(DESTDIR)$(datadir)/doc/wolframe
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/man/man5
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/man/man8
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/man/man1
	$(INSTALL) -m 0644 wolframe.conf.5 $(DESTDIR)$(datadir)/man/man5
	gzip -f $(DESTDIR)$(datadir)/man/man5/wolframe.conf.5
	$(INSTALL) -m 0644 wolframed.8 $(DESTDIR)$(datadir)/man/man8
	gzip -f $(DESTDIR)$(datadir)/man/man8/wolframed.8
	$(INSTALL) -m 0644 wolfpasswd.1 $(DESTDIR)$(datadir)/man/man1
	gzip -f $(DESTDIR)$(datadir)/man/man1/wolfpasswd.1

local_uninstall:
	@-rm -rf $(DESTDIR)$(datadir)/doc/wolframe/html
	@-rmdir $(DESTDIR)$(datadir)/doc/wolframe
	@-rmdir $(DESTDIR)$(datadir)/doc
	@-rm $(DESTDIR)$(datadir)/man/man1/wolfpasswd.1.gz
	@-rmdir $(DESTDIR)$(datadir)/man/man1
	@-rm $(DESTDIR)$(datadir)/man/man8/wolframed.8.gz
	@-rmdir $(DESTDIR)$(datadir)/man/man8
	@-rm $(DESTDIR)$(datadir)/man/man5/wolframe.conf.5.gz
	@-rmdir $(DESTDIR)$(datadir)/man/man5
	@-rmdir $(DESTDIR)$(datadir)/man
	@-rmdir $(DESTDIR)$(datadir)

check:
	xmllint --noout --xinclude --postvalid WolfBook.xml
	xmllint --noout --xinclude --postvalid wolframe.conf-man.xml
	xmllint --noout --xinclude --postvalid wolframed-man.xml
	xmllint --noout --xinclude --postvalid Tutorial.xml
