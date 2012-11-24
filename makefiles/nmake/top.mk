# top-level makefile for a package
#
# requires:
# - TOPDIR
# - SUBDIRS
#
# provides:
# - target 'all'
# - target 'clean'
# - target 'distclean'
# - target 'test'
# - target 'longtest'
# - target 'doc'
# - target 'help'

all:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'all' & cd %%d & $(MAKE) /nologo /f Makefile.w32 all & cd ..

clean:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'clean' & cd %%d & $(MAKE) /nologo /f Makefile.w32 clean & cd ..

distclean:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'distclean' & cd %%d & $(MAKE) /nologo /f Makefile.w32 distclean & cd ..

test: all
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'test' & cd %%d & $(MAKE) /nologo /f Makefile.w32 test & cd ..

longtest: test
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'longtest' & cd %%d & $(MAKE) /nologo /f Makefile.w32 longtest & cd ..

doc:
	@cd docs & $(MAKE) /nologo /f Makefile.W32 doc

help:
	@type makefiles\nmake\help.mk
