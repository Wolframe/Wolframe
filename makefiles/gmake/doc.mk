# makefile for a sub package
#
# requires:
# - TOPDIR
# - SUBDIRS
# - INCLUDE_DIRS
#
# provides:
# - target: all targets

-include $(TOPDIR)/makefiles/gmake/platform.mk
-include $(TOPDIR)/makefiles/gmake/install_dirs.mk

.PHONY: all $(SUBDIRS) local_all
all: local_all
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d all || exit 1); done)

.PHONY: test local_test
test: local_test
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d test || exit 1); done)

.PHONY: longtest local_longtest
longtest: local_longtest
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d longtest || exit 1); done)

.PHONY: help
help:
	@cat $(TOPDIR)/makefiles/gmake/dochelp.mk

.PHONY: config
config:
	@echo "Documentation Build Configuration for Wolframe"
	@echo "----------------------------------------------"
	@echo
	@echo "Operating system: $(PLATFORM), $(OS_MAJOR_VERSION).$(OS_MINOR_VERSION)"
ifeq "$(PLATFORM)" "LINUX"
	@echo "Linux distribution: $(LINUX_DIST) $(LINUX_REV)"
endif
	@echo
	@echo "XSLT_MAN_STYLESHEET: $(XSLT_MAN_STYLESHEET)"

-include $(TOPDIR)/makefiles/gmake/clean.mk
-include $(TOPDIR)/makefiles/gmake/install.mk
-include $(TOPDIR)/makefiles/gmake/i18n.mk
