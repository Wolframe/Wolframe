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
# - target 'doc'
# - target 'dist'
# - target 'help'

all:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 all & cd ..

clean:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 clean & cd ..

distclean:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 distclean & cd ..

#.PHONY: install
#install:
#	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
#	  (set -e; $(MAKE) -C $$d install || exit 1); done)

test: all
	@cd tests & $(MAKE) /nologo /f Makefile.W32 test

doc:
	@cd docs & $(MAKE) /nologo /f Makefile.W32 doc

help:
	@type makefiles\nmake\help.mk

#-include $(TOPDIR)/makefiles/gmake/dist.mk
