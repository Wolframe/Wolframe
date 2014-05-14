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
# - target 'dist'
# - target 'help'
# - target 'install'
# - target 'uninstall'
# - target 'config'
# - target 'depend'

-include makefiles/gmake/platform.mk
-include makefiles/gmake/compiler.mk
-include makefiles/gmake/install_dirs.mk

.PHONY: all
all:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d all || exit 1); done)

.PHONY: clean
clean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d clean || exit 1); done)
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.mk.vars || rm $(TOPDIR)/makefiles/gmake/platform.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.vars || rm $(TOPDIR)/makefiles/gmake/platform.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/python.mk.vars || rm $(TOPDIR)/makefiles/gmake/python.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/python.vars || rm $(TOPDIR)/makefiles/gmake/python.vars

.PHONY: distclean
distclean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d distclean || exit 1); done)
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.mk.vars || rm $(TOPDIR)/makefiles/gmake/platform.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.vars || rm $(TOPDIR)/makefiles/gmake/platform.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/python.mk.vars || rm $(TOPDIR)/makefiles/gmake/python.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/python.vars || rm $(TOPDIR)/makefiles/gmake/python.vars

.PHONY: install
install:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d install || exit 1); done)

.PHONY: uninstall
uninstall:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

.PHONY: depend
depend:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d depend || exit 1); done)

.PHONY: test
test: all
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d test || exit 1); done)

.PHONY: testreport
testreport:
	WOLFRAME_TESTREPORT_DIR=$(PWD)/tests/reports/ \
	GTEST_OUTPUT=xml:$(PWD)/tests/reports/ \
		$(MAKE) test
	@tests/reports/generateTestReport.sh

.PHONY: longtest
longtest: test
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d longtest || exit 1); done)

.PHONY: help
help:
	@cat $(TOPDIR)/makefiles/gmake/help.mk

.PHONY: config
config:
	@echo "Build Configuration for Wolframe"
	@echo "--------------------------------"
	@echo
	@echo "Operating system: $(PLATFORM), $(OS_MAJOR_VERSION).$(OS_MINOR_VERSION)"
	@echo "Architecture: $(ARCH)"
ifeq "$(PLATFORM)" "LINUX"
	@echo "Linux distribution: $(LINUX_DIST) $(LINUX_REV)"
endif
ifeq ($(RELEASE),1)
	@echo "Building release version"
else
	@echo "Building debug version"
endif
	@echo "C++ Compiler: $(COMPILER)"
	@echo "Optimization flags: $(OPTFLAGS)"
	@echo "Full C compilation flags: $(ALL_CFLAGS)"
	@echo "Full C++ compilation flags: $(ALL_CXXFLAGS)"
	@echo "Global linker flags: $(ALL_LDFLAGS)"
	@echo "Global libraries: $(LIBS)"
	@echo "Libraries for module loading via dlopen: $(LIBS_DL)"
	@echo "Libraries for networking: $(LIBS_NET)"
	@echo "Compilation flags for i18n support: $(INCLUDE_FLAGS_LT)"
	@echo "Linking flags for i18n support: $(LDFLAGS_LT)"
	@echo "Libraries for i18n support: $(LIBS_LT)"
	@echo
	@echo "Required Dependencies:"
	@echo
	@echo "BOOST_DIR: $(BOOST_DIR)"
	@echo "BOOST_INCLUDE_DIR: $(BOOST_INCLUDE_DIR)"
	@echo "BOOST_LIB_DIR: $(BOOST_LIB_DIR)"
	@echo "BOOST_LIBRARY_TAG: $(BOOST_LIBRARY_TAG)"
	@echo
	@echo "Optional Dependencies:"
ifeq ($(WITH_SSL),1)
	@echo
	@echo "OPENSSL_DIR: $(OPENSSL_DIR)"
	@echo "OPENSSL_INCLUDE_DIR: $(OPENSSL_INCLUDE_DIR)"
	@echo "OPENSSL_LIB_DIR: $(OPENSSL_LIB_DIR)"
	@echo "OPENSSL_LIBS: $(OPENSSL_LIBS)"
endif
ifeq ($(WITH_LUA),1)
	@echo
	@echo "LUA_PLATFORM_CFLAGS: $(LUA_PLATFORM_CFLAGS)"
	@echo "LUA_PLATFORM_LDFLAGS: $(LUA_PLATFORM_LDFLAGS)"
	@echo "LUA_PLATFORM_LIBS: $(LUA_PLATFORM_LIBS)"
endif
ifeq ($(WITH_PAM),1)
	@echo
	@echo "PAM_DIR: $(PAM_DIR)"
	@echo "PAM_INCLUDE_DIR: $(PAM_INCLUDE_DIR)"
	@echo "PAM_LIB_DIR: $(PAM_LIB_DIR)"
	@echo "PAM_LIBS: $(PAM_LIBS)"
endif
ifeq ($(WITH_SASL),1)
	@echo
	@echo "SASL_DIR: $(SASL_DIR)"
	@echo "SASL_INCLUDE_DIR: $(SASL_INCLUDE_DIR)"
	@echo "SASL_LIB_DIR: $(SASL_LIB_DIR)"
	@echo "SASL_LIBS: $(SASL_LIBS)"
endif
ifeq ($(WITH_SQLITE3),1)
ifeq ($(WITH_SYSTEM_SQLITE3),1)
	@echo
	@echo "SQLITE3_DIR: $(SQLITE3_DIR)"
	@echo "SQLITE3_INCLUDE_DIR: $(SQLITE3_INCLUDE_DIR)"
	@echo "SQLITE3_LIB_DIR: $(SQLITE3_LIB_DIR)"
	@echo "SQLITE3_LIBS: $(SQLITE3_LIBS)"
endif
ifeq ($(WITH_LOCAL_SQLITE3),1)
	@echo
	@echo "using local Sqlite3 amalgamation sources.."
endif
endif
ifeq ($(WITH_PGSQL),1)
	@echo
	@echo "PGSQL_DIR: $(PGSQL_DIR)"
	@echo "PGSQL_INCLUDE_DIRS: $(PGSQL_INCLUDE_DIRS)"
	@echo "PGSQL_LIB_DIRS: $(PGSQL_LIB_DIRS)"
	@echo "PGSQL_LIBS: $(PGSQL_LIBS)"
endif
ifeq ($(WITH_ORACLE),1)
	@echo
	@echo "ORACLE_DIR: $(ORACLE_DIR)"
	@echo "ORACLE_INCLUDE_DIRS: $(ORACLE_INCLUDE_DIRS)"
	@echo "ORACLE_LIB_DIRS: $(ORACLE_LIB_DIRS)"
	@echo "ORACLE_LIBS: $(ORACLE_LIBS)"
endif
ifeq ($(WITH_TEXTWOLF),1)
	@echo
	@echo "using local textwolf library for charset detection and XML filtering module.."
endif
ifeq ($(WITH_LIBXML2),1)
	@echo
	@echo "LIBXML2_DIR: $(LIBXML2_DIR)"
	@echo "LIBXML2_INCLUDE_DIRS: $(LIBXML2_INCLUDE_DIRS)"
	@echo "LIBXML2_LIB_DIRS: $(LIBXML2_LIB_DIRS)"
	@echo "LIBXML2_LIBS: $(LIBXML2_LIBS)"
endif
ifeq ($(WITH_LIBXSLT),1)
	@echo
	@echo "LIBXSLT_DIR: $(LIBXSLT_DIR)"
	@echo "LIBXSLT_INCLUDE_DIRS: $(LIBXSLT_INCLUDE_DIRS)"
	@echo "LIBXSLT_LIB_DIRS: $(LIBXSLT_LIB_DIRS)"
	@echo "LIBXSLT_LIBS: $(LIBXSLT_LIBS)"
endif
ifeq ($(WITH_LIBHPDF),1)
ifeq ($(WITH_SYSTEM_LIBHPDF),1)
	@echo
	@echo "LIBHPDF_DIR: $(LIBHPDF_DIR)"
	@echo "LIBHPDF_INCLUDE_DIRS: $(LIBHPDF_INCLUDE_DIRS)"
	@echo "LIBHPDF_LIB_DIRS: $(LIBHPDF_LIB_DIRS)"
	@echo "LIBHPDF_LIBS: $(LIBHPDF_LIBS)"
endif
ifeq ($(WITH_LOCAL_LIBHPDF),1)
	@echo
	@echo "using local libhpdf.."
	@echo
	@echo "LIBPNG_DIR: $(LIBPNG_DIR)"
	@echo "LIBPNG_INCLUDE_DIRS: $(LIBPNG_INCLUDE_DIRS)"
	@echo "LIBPNG_LIB_DIRS: $(LIBPNG_LIB_DIRS)"
	@echo "LIBPNG_LIBS: $(LIBPNG_LIBS)"
	@echo
	@echo "LIBZ_DIR: $(LIBZ_DIR)"
	@echo "LIBZ_INCLUDE_DIRS: $(LIBZ_INCLUDE_DIRS)"
	@echo "LIBZ_LIB_DIRS: $(LIBZ_LIB_DIRS)"
	@echo "LIBZ_LIBS: $(LIBZ_LIBS)"
endif
endif
ifeq ($(WITH_ICU),1)
	@echo
	@echo "using ICU (icu4c) as boost locale backend"
	@echo
	@echo "ICU_DIR: $(ICU_DIR)"
	@echo "ICU_INCLUDE_DIRS: $(ICU_INCLUDE_DIRS)"
	@echo "ICU_LIB_DIRS: $(ICU_LIB_DIRS)"
	@echo "ICU_LIBS: $(ICU_LIBS)"
endif
ifeq ($(WITH_FREEIMAGE),1)
ifeq ($(WITH_SYSTEM_FREEIMAGE),1)
	@echo
	@echo "building module using FreeImage, for image manipulation"
	@echo
	@echo "FREEIMAGE_DIR: $(FREEIMAGE_DIR)"
	@echo "FREEIMAGE_INCLUDE_DIRS: $(FREEIMAGE_INCLUDE_DIRS)"
	@echo "FREEIMAGE_LIB_DIRS: $(FREEIMAGE_LIB_DIRS)"
	@echo "FREEIMAGE_LIBS: $(FREEIMAGE_LIBS)"
	@echo "FREEIMAGEPLUS_INCLUDE_DIRS: $(FREEIMAGEPLUS_INCLUDE_DIRS)"
	@echo "FREEIMAGEPLUS_LIB_DIRS: $(FREEIMAGEPLUS_LIB_DIRS)"
	@echo "FREEIMAGEPLUS_LIBS: $(FREEIMAGEPLUS_LIBS)"
endif
ifeq ($(WITH_LOCAL_FREEIMAGE),1)
	@echo
	@echo "using local FreeImage for image manipulation module.."
endif
endif
ifeq ($(WITH_PYTHON),1)
	@echo
	@echo "enabled Python bindings"
	@echo
	@echo "PYTHON_CFLAGS: $(PYTHON_CFLAGS)"
	@echo "PYTHON_LDFLAGS: $(PYTHON_LDFLAGS)"
	@echo "PYTHON_LIBS: $(PYTHON_LIBS)"
endif
ifeq ($(WITH_CJSON),1)
	@echo
	@echo "using local cjson library for JSON filtering module.."
endif
	@echo
	@echo "Installation directories"
	@echo
	@echo "DESTDIR: $(DESTDIR)"
	@echo "prefix: $(prefix)"
	@echo "execdir: $(execdir)"
	@echo "bindir: $(bindir)"
	@echo "sbindir: $(sbindir)"
	@echo "libdir: $(libdir)"
	@echo "sysconfdir: $(sysconfdir)"
	@echo "includedir: $(includedir)"
	@echo "datadir: $(datadir)"
	@echo "mandir: $(mandir)"
	@echo "localedir: $(localedir)"
	@echo "moduleloaddir: $(moduleloaddir)"
	@echo
	@echo "Additional build options:"
	@echo
ifeq ($(ENABLE_NLS),0)
	@echo "NLS support is disabled"
else
	@echo "NLS support is enabled"
endif
ifeq ($(RUN_TESTS),0)
	@echo "Will not execute any tests"
endif
ifeq ($(WITH_EXPECT),1)
	@echo "Will execute expect tests"
endif
	@echo

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
