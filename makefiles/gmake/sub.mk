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

.PHONY: all $(SUBDIRS) local_all
all: $(OBJS) $(CPPOBJS) $(BIN_OBJS) $(CPP_BIN_OBJS) $(BINS) $(CPP_BINS) $(CMODULES) $(CPPMODULES) $(STATIC_LIB) $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) local_all all_po
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d all || exit 1); done)

.PHONY: test local_test
test: $(OBJS) $(TEST_OBJS) $(CPPOBJS) $(BIN_OBJS) $(BINS) $(CPP_BINS) $(TEST_BIN_OBJS) $(TEST_BINS) $(TEST_CPP_BINS) $(CMODULES) $(CPPMODULES) $(STATIC_LIB) local_test
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d test || exit 1); done)

-include $(TOPDIR)/makefiles/gmake/depend.mk
-include $(TOPDIR)/makefiles/gmake/clean.mk
-include $(TOPDIR)/makefiles/gmake/install.mk
-include $(TOPDIR)/makefiles/gmake/i18n.mk
-include $(TOPDIR)/makefiles/gmake/libs.mk
