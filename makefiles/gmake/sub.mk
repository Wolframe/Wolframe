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
-include $(TOPDIR)/makefiles/gmake/compiler.mk
-include $(TOPDIR)/makefiles/gmake/install_dirs.mk

.PHONY: all subdirs $(SUBDIRS) local_all
all: subdirs depend $(BINS) $(CPP_BINS) $(STATIC_LIB) $(NOINST_STATIC_LIB) $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR) $(DYNAMIC_LIB) $(DYNAMIC_MODULE) local_all all_po
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ all

.PHONY: test local_test
test: all local_test
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d test || exit 1); done)

.PHONY: longtest local_longtest
longtest: test local_longtest
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d longtest || exit 1); done)

-include $(TOPDIR)/makefiles/gmake/depend.mk
-include $(TOPDIR)/makefiles/gmake/clean.mk
-include $(TOPDIR)/makefiles/gmake/install.mk
-include $(TOPDIR)/makefiles/gmake/i18n.mk
-include $(TOPDIR)/makefiles/gmake/libs.mk
