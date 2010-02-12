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
# - target 'install'
# - target 'uninstall'

-include makefiles/gmake/platform.mk

.PHONY: all
all:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d all || exit 1); done)

.PHONY: clean
clean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d clean || exit 1); done)
	@-rm $(TOPDIR)/makefiles/gmake/platform.mk.vars

.PHONY: distclean
distclean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d distclean || exit 1); done)
	@-rm $(TOPDIR)/makefiles/gmake/platform.mk.vars

.PHONY: install
install:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d install || exit 1); done)

.PHONY: uninstall
uninstall:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: doc
doc:
	cd docs; $(MAKE) doc

.PHONY: help
help:
	@cat $(TOPDIR)/makefiles/gmake/help.mk

.PHONY: init-po
init-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d init-po || exit 1); done)

.PHONY: merge-po
merge-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d merge-po || exit 1); done)

.PHONY: check-po
check-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d check-po || exit 1); done)

-include $(TOPDIR)/makefiles/gmake/dist.mk
