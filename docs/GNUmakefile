TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/doc.mk

XSLT_MAN = \
	$(XSLTPROC) --nonet --xinclude \
	--param man.charmap.use.subset "0" \
	--param make.year.ranges "1" \
	--param make.single.year.ranges "1" \
	$(XSLT_MAN_STYLESHEET)

XSLT_HTML = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	output-html.xsl

XSLT_HTML_CHUNKED = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	-o $@ output-html-chunked.xsl

XSLT_WEBPAGE_HTML = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode no \
	--stringparam toc.section.depth 4 \
	--stringparam base.dir $(@D) \
	output-html-webpage.xsl

XSLT_WEBPAGE_HTML_CHUNKED = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode no \
	--stringparam toc.section.depth 4 \
	-o $@ output-html-webpage-chunked.xsl

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

XSLT_TOWORDML = \
	$(XSLTPROC) --nonet --xinclude \
	--stringparam draft.mode yes \
	--stringparam toc.section.depth 4 \
	-stringparam wordml.template $(XSLT_TOWORDML_TEMPLATE) \
	$(XSLT_TOWORDML_STYLESHEET)
	
local_all:

doc: doc-doxygen doc-html doc-html-chunked doc-pdf doc-txt

MAN_PAGES = \
	wolframed.8 \
	wolframe.conf.5 \
	wolfpasswd.1 \
	wolframe-mod_db_sqlite3.5
	
man: $(MAN_PAGES)

doc-doxygen:
	cd $(TOPDIR); doxygen docs/doxygen.conf

doc-doxygen-webpage:
	@-mkdir doxygen/html-webpage
	cd $(TOPDIR); doxygen docs/doxygen-webpage.conf

doc-html: WolfBook.html Tutorial.html Installation.html

doc-txt: InstallationUnix.txt

doc-html-chunked: html/index.html html-tutorial/index.html html-installation/index.html

doc-html-webpage: html-webpage/index.html html-tutorial-webpage/index.html html-installation-webpage/index.html

doc-html-webpage-chunked: html-webpage-chunked/index.html html-tutorial-webpage-chunked/index.html html-installation-webpage-chunked/index.html

doc-pdf: WolfBook.pdf Tutorial.pdf Installation.pdf

.PHONY: depend
depend:

doc-epub: WolfBook.epub Tutorial.epub Installation.epub

doc-towordml: WolfBook.docx

%.html: %.xml
	$(XSLT_HTML) $< > $@

%.pdf: %.fop
	fop -fo $< -pdf $@

%.fop: %.xml
	$(XSLT_PDF) $< > $@

%.epub: %.xml
	$(XSLT_EPUB) $<

%.docx: %.xml
	$(XSLT_TOWORDML) $< > $@

%.txt: %.html
	w3m -O ASCII -dump $< > $@
	@sed 's/Copyright ?/Copyright (C)/g' $@ > _tmp
	@mv -f _tmp $@ 

# Installation Guide

INSTALLATION_FILES = \
	Installation.xml \
	InstallationVersion.txt \
	InstallationDate.txt \
	Installation/Preface.xml \
	Installation/InstallationSource.xml \
	Installation/InstallationSourceUnix.xml \
	Installation/InstallationSourceUnixClient.xml \
	Installation/InstallationSourceWindows.xml \
	Installation/InstallationSourceWindowsClient.xml \
	Installation/InstallationBinary.xml \
	WolframeVersion.txt \
	WolfclientVersion.txt

INSTALLATION_UNIX_FILES = \
	InstallationUnix.xml \
	InstallationVersion.txt \
	InstallationDate.txt \
	Installation/InstallationSourceUnix.xml \
	WolframeVersion.txt \
	WolfclientVersion.txt

INSTALLATION_WINDOWS_FILES = \
	InstallationWindows.xml \
	InstallationVersion.txt \
	InstallationDate.txt \
	Installation/InstallationSourceWindows.xml \
	WolframeVersion.txt \
	WolfclientVersion.txt

INSTALLATION_UNIX_CLIENT_FILES = \
	InstallationUnixClient.xml \
	InstallationVersion.txt \
	InstallationDate.txt \
	Installation/InstallationSourceUnixClient.xml \
	WolframeVersion.txt \
	WolfclientVersion.txt

INSTALLATION_WINDOWS_CLIENT_FILES = \
	InstallationWindowsClient.xml \
	InstallationVersion.txt \
	InstallationDate.txt \
	Installation/InstallationSourceWindowsClient.xml \
	WolframeVersion.txt \
	WolfclientVersion.txt
	
Installation.html: $(INSTALLATION_FILES)
Installation.pdf: $(INSTALLATION_FILES)

InstallationUnix.html: $(INSTALLATION_UNIX_FILES)
InstallationWindows.html: $(INSTALLATION_WINDOWS_FILES)

InstallationUnixClient.html: $(INSTALLATION_UNIX_CLIENT_FILES)
InstallationWindowsClient.html: $(INSTALLATION_WINDOWS_CLIENT_FILES)

html-installation/index.html: $(INSTALLATION_FILES)
	$(XSLT_HTML_CHUNKED) $<

html-installation-webpage/index.html: $(INSTALLATION_FILES)
	$(XSLT_WEBPAGE_HTML) $<

html-installation-webpage-chunked/index.html: $(INSTALLATION_FILES)
	$(XSLT_WEBPAGE_HTML_CHUNKED) $<

Installation.epub: $(INSTALLATION_FILES)
	mkdir -p OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  Installation.epub mimetype
	zip -Xr9D Installation.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS

# WolfBook

WOLFBOOK_SOURCE_FILES = \
	WolfBook.xml \
	WolfBookVersion.txt \
	WolfBookDate.txt \
	AppDevelopment/AppDevelopment.ent \
	AppDevelopment/Appendix.xml \
	AppDevelopment/CmdHandlerDirectmap.xml \
	AppDevelopment/CmdHandlerLua.xml \
	AppDevelopment/CmdHandlers.xml \
	AppDevelopment/EBNF_Normalizer.xml \
	AppDevelopment/EBNF_TransactionDefinitionLanguage.xml \
	AppDevelopment/FilterJSON.xml \
	AppDevelopment/FilterXML.xml \
	AppDevelopment/FilterXSLT.xml \
	AppDevelopment/Filters.xml \
	AppDevelopment/Forms.xml \
	AppDevelopment/Functions.xml \
	AppDevelopment/Glossary.xml \
	AppDevelopment/Introduction.xml \
	AppDevelopment/ModelTypedFilter.xml \
	AppDevelopment/Preface.xml \
	AppDevelopment/ProgramCpp.xml \
	AppDevelopment/ProgramDotNet.xml \
	AppDevelopment/ProgramFormDataTypes.xml \
	AppDevelopment/ProgramForms.xml \
	AppDevelopment/ProgramLua.xml \
	AppDevelopment/ProgramPython.xml \
	AppDevelopment/ProgramTransactions.xml \
	AppDevelopment/README \
	AppDevelopment/Testing.xml \
	Client/CLI.xml \
	Client/Client.ent \
	Client/Introduction.xml \
	Client/Preface.xml \
	Client/Wolfclient.xml \
	Client/WolfclientArchitecture.xml \
	Client/WolfclientArtifacts.xml \
	Client/WolfclientDebugging.xml \
	Client/WolfclientProgrammingInterface.xml \
	Client/WolfclientRequestAnswer.xml \
	Common/Common.ent \
	Common/WolframeLegalNotice.xml \
	Common/gpl.dbk \
	ExtDevelopment/Appendix.xml \
	ExtDevelopment/BasicDataTypes.xml \
	ExtDevelopment/ExtDevelopment.ent \
	ExtDevelopment/Glossary.xml \
	ExtDevelopment/Introduction.xml \
	ExtDevelopment/NormalizerModules.xml \
	ExtDevelopment/Preface.xml

WolfBook.html: $(WOLFBOOK_SOURCE_FILES)

html/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_HTML_CHUNKED) $<

html-webpage/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML) $<

html-webpage-chunked/index.html: $(WOLFBOOK_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML_CHUNKED) $<

WolfBook.epub: $(WOLFBOOK_SOURCE_FILES)
	mkdir -p OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  WolfBook.epub mimetype
	zip -Xr9D WolfBook.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS

# tutorial

TUTORIAL_SOURCE_FILES = \
	Tutorial.xml \
	TutorialVersion.txt \
	TutorialDate.txt \
	Tutorial/Tutorial.ent \
	Tutorial/Preface.xml \
	Tutorial/Introduction.xml \
	Tutorial/Step1.xml \
	Tutorial/Step2.xml \
	Tutorial/Step3.xml \
	Tutorial/Step4.xml \
	Tutorial/Step5.xml \
	Tutorial/Step6.xml \
	Tutorial/Step7.xml \
	Tutorial/Step8.xml \
	Tutorial/Step9.xml

Tutorial.html: $(TUTORIAL_SOURCE_FILES)
Tutorial.pdf: $(TUTORIAL_SOURCE_FILES)

html-tutorial/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_HTML_CHUNKED) $<

html-tutorial-webpage/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML) $<

html-tutorial-webpage-chunked/index.html: $(TUTORIAL_SOURCE_FILES)
	$(XSLT_WEBPAGE_HTML_CHUNKED) $<

Tutorial.epub: $(TUTORIAL_SOURCE_FILES)
	mkdir -p OEBPS/images
	cp images/draft.png images/*.svg OEBPS/images
	echo "application/epub+zip" > mimetype
	zip -0Xq  Tutorial.epub mimetype
	zip -Xr9D Tutorial.epub META-INF OEBPS
	rm -rf mimetype META-INF OEBPS

# man pages

%.8: %-man.xml
	$(XSLT_MAN) $<

%.5: %-man.xml
	$(XSLT_MAN) $<

%.1: %-man.xml
	$(XSLT_MAN) $<

local_clean:
	-rm -rf doxygen/html/* doxygen/man/*  doxygen/perlmod/*
	-rm -f WolfBook.html WolfBook.fop WolfBook.pdf WolfBook.epub WolfBook.docx
	-rm -f Tutorial.html Tutorial.pdf
	-rm -f Installation.html Installation.pdf InstallationUnix.html InstallationUnix.txt InstallationWindows.html InstallationWindows.txt
	-rm -rf html/*.html
	-rm -rf html-webpage/*.html
	-rm -rf html-tutorial/*.html
	-rm -rf html-installation/*.html
	-rm -rf html-webpage-chunked/*.html
	-rm -rf html-tutorial-webpage/*.html
	-rm -rf html-tutorial-webpage-chunked/*.html
	-rm -rf html-installation-webpage/*.html
	-rm -rf html-installation-webpage-chunked/*.html
	-rm -rf doxygen/html-webpage/*
	-rm -rf InstallationUnixClient.html InstallationUnixClient.txt
	-rm -rf InstallationWindowsClient.html InstallationWindowsClient.txt
	
local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(datadir)/doc/wolframe
	-test -d doxygen/html && cp -R doxygen/html/ $(DESTDIR)$(datadir)/doc/wolframe
	$(INSTALL) -d -m 0755 $(DESTDIR)$(mandir)/man5
	$(INSTALL) -d -m 0755 $(DESTDIR)$(mandir)/man8
	$(INSTALL) -d -m 0755 $(DESTDIR)$(mandir)/man1
	$(INSTALL) -m 0644 wolframe.conf.5 $(DESTDIR)$(mandir)/man5
ifneq "$(PLATFORM)" "SUNOS"
	gzip -f $(DESTDIR)$(mandir)/man5/wolframe.conf.5
endif
	$(INSTALL) -m 0644 wolframe-mod_db_sqlite3.5 $(DESTDIR)$(mandir)/man5
ifneq "$(PLATFORM)" "SUNOS"
	gzip -f $(DESTDIR)$(mandir)/man5/wolframe-mod_db_sqlite3.5
endif
	$(INSTALL) -m 0644 wolframed.8 $(DESTDIR)$(mandir)/man8
ifneq "$(PLATFORM)" "SUNOS"
	gzip -f $(DESTDIR)$(mandir)/man8/wolframed.8
endif
	$(INSTALL) -m 0644 wolfpasswd.1 $(DESTDIR)$(mandir)/man1
ifneq "$(PLATFORM)" "SUNOS"
	gzip -f $(DESTDIR)$(mandir)/man1/wolfpasswd.1
endif

local_uninstall:
	@-rm -rf $(DESTDIR)$(datadir)/doc/wolframe/html
	@-rmdir $(DESTDIR)$(datadir)/doc/wolframe
	@-rmdir $(DESTDIR)$(datadir)/doc
	@-rmdir $(DESTDIR)$(datadir)
	@-rm $(DESTDIR)$(mandir)/man1/wolfpasswd.1*
	@-rmdir $(DESTDIR)$(mandir)/man1
	@-rm $(DESTDIR)$(mandir)/man8/wolframed.8*
	@-rmdir $(DESTDIR)$(mandir)/man8
	@-rm $(DESTDIR)$(mandir)/man5/wolframe.conf.5*
	@-rm $(DESTDIR)$(mandir)/man5/wolframe-mod_db_sqlite3.5*
	@-rmdir $(DESTDIR)$(mandir)/man5
	@-rmdir $(DESTDIR)$(mandir)

check:
	xmllint --noout --xinclude --postvalid WolfBook.xml
	xmllint --noout --xinclude --postvalid Tutorial.xml
	xmllint --noout --xinclude --postvalid Installation.xml
	xmllint --noout --xinclude --postvalid wolframe.conf-man.xml
	xmllint --noout --xinclude --postvalid wolframed-man.xml
	xmllint --noout --xinclude --postvalid wolfpasswd-man.xml

