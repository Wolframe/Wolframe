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
# - target 'config'

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

.PHONY: config
config:
	@echo "Build Configuration for SMERP"
	@echo "-----------------------------"
	@echo
	@echo "Operating system: $(PLATFORM), $(OS_MAJOR_VERSION).$(OS_MINOR_VERSION)"
ifeq "$(PLATFORM)" "LINUX"
	@echo "Linux distribution: $(LINUX_DIST) $(LINUX_REV)"
endif
	@echo "C++ Compiler: $(COMPILER)"
	@echo
	@echo "Required Dependencies:"
	@echo
	@echo "BOOST_INCLUDE_DIRS: $(BOOST_INCLUDE_DIRS)"
	@echo "BOOST_LIB_DIR: $(BOOST_LIB_DIR)"
	@echo "BOOST_LIBRARY_TAG: $(BOOST_LIBRARY_TAG)"
	@echo
	@echo "Optional Dependencies:"
	@echo
ifdef WITH_LUA
	@echo "LUA_INCLUDE_DIRS: $(LUA_INCLUDE_DIRS)"
	@echo "LUA_LIB_DIRS: $(LUA_LIB_DIRS)"
	@echo "LUA_LIBS: $(LUA_LIBS)"
endif
ifdef WITH_QT
	@echo "QT_DIR: $(QT_DIR)"
	@echo "QT_INCLUDE_DIRS: $(QT_INCLUDE_DIRS)"
	@echo "QT_LIB_DIRS: $(QT_LIB_DIRS)"
endif

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
