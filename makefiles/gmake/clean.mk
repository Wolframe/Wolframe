# cleans up directories
#
# requires:
# - SUBDIRS: for recursive cleaning
# - local_clean, local_distclean targets in local GNUmakefile
# - all artifacts to clean:
#   - BINS, TEST_BINS, TEST_CPP_BINS, CPP_BINS
#   - OBJS, CPP_OBJS, BIN_OBJS, TEST_BIN_OBJS, CPP_BIN_OBJS, TEST_CPP_BIN_OBJS
#
# provides:
# - target: clean
# - target: distclean

.PHONY: clean_recursive clean local_clean

clean_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d clean || exit 1); done)

clean: clean_recursive clean_po local_clean
	-@rm -f *.bak 2>/dev/null
	-@rm -f *~ 2>/dev/null
	-@rm -f *.d */*.d */*/*.d */*/*/*.d */*/*/*/*.d 2>/dev/null
	-@rm -f $(BINS) $(CPP_BINS) $(TEST_BINS) $(TEST_CPP_BINS) 2>/dev/null
	-@rm -f $(OBJS) $(CPP_OBJS) $(BIN_OBJS) $(TEST_BIN_OBJS) $(CPP_BIN_OBJS) $(TEST_CPP_BIN_OBJS) 2>/dev/null
	-@rm -f exec/* 2>/dev/null
	-@rm -f *.core  2>/dev/null
	-@rm -rf $(STATIC_LIB) 2>/dev/null
	-@rm -rf $(NOINST_STATIC_LIB) 2>/dev/null
ifneq "$(DYNAMIC_LIB)" ""
	-@rm -rf $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) 2>/dev/null
	-@rm -rf $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR) 2>/dev/null
	-@rm -rf $(DYNAMIC_LIB) 2>/dev/null
endif
ifneq "$(NOINST_DYNAMIC_LIB)" ""
	-@rm -rf $(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH) 2>/dev/null
	-@rm -rf $(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR) 2>/dev/null
	-@rm -rf $(NOINST_DYNAMIC_LIB) 2>/dev/null
endif
ifneq "$(DYNAMIC_MODULE)" ""
	-@rm -rf $(DYNAMIC_MODULE) 2>/dev/null
endif
ifneq "$(NOINST_DYNAMIC_MODULE)" ""
	-@rm -rf $(NOINST_DYNAMIC_MODULE) 2>/dev/null
endif


.PHONY: distclean_recursive distclean local_distclean

distclean_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d distclean || exit 1); done)

distclean: distclean_recursive local_distclean clean
