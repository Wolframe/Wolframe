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

.PHONY: help
help:
	@cat $(TOPDIR)/makefiles/gmake/help.mk

.PHONY: config
config:
	@echo "Build Configuration for Wolframe"
	@echo "--------------------------------"
	@echo
	@echo "Operating system: $(PLATFORM), $(OS_MAJOR_VERSION).$(OS_MINOR_VERSION)"
ifeq "$(PLATFORM)" "LINUX"
	@echo "Linux distribution: $(LINUX_DIST) $(LINUX_REV)"
endif
	@echo "C++ Compiler: $(COMPILER)"
	@echo
	@echo "Required Dependencies:"
	@echo
	@echo "BOOST_DIR: $(BOOST_DIR)"
	@echo "BOOST_INCLUDE_DIR: $(BOOST_INCLUDE_DIR)"
	@echo "BOOST_LIB_DIR: $(BOOST_LIB_DIR)"
	@echo "BOOST_LIBRARY_TAG: $(BOOST_LIBRARY_TAG)"
	@echo
	@echo "Optional Dependencies:"
ifdef WITH_SSL
	@echo
	@echo "OPENSSL_DIR: $(OPENSSL_DIR)"
	@echo "OPENSSL_LIBS: $(OPENSSL_LIBS)"
endif
ifdef WITH_LUA
	@echo
	@echo "LUA_INCLUDE_DIR: $(LUA_INCLUDE_DIR)"
	@echo "LUA_LIB_DIR: $(LUA_LIB_DIR)"
	@echo "LUA_LIBS: $(LUA_LIBS)"
endif
ifdef WITH_QT
	@echo
	@echo "QT_DIR: $(QT_DIR)"
	@echo "QT_INCLUDE_DIR: $(QT_INCLUDE_DIR)"
	@echo "QT_LIB_DIR: $(QT_LIB_DIR)"
endif
ifdef WITH_GTEST
	@echo
	@echo "GTEST_DIR: $(GTEST_DIR)"
	@echo "GTEST_INCLUDE_DIR: $(GTEST_INCLUDE_DIR)"
	@echo "GTEST_LIB_DIR: $(GTEST_LIB_DIR)"
	@echo "GTEST_LIBS: $(GTEST_LIBS)"
endif
ifdef WITH_PAM
	@echo
	@echo "PAM_DIR: $(PAM_DIR)"
	@echo "PAM_INCLUDE_DIR: $(PAM_INCLUDE_DIR)"
	@echo "PAM_LIB_DIR: $(PAM_LIB_DIR)"
	@echo "PAM_LIBS: $(PAM_LIBS)"
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
