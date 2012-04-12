TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBGMP_INCLUDE_DIRS) \
	$(MPFR_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(LIBGMP_LIB_DIRS) \
	$(MPFR_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBGMP_LIBS) \
	$(MPFR_LIBS)

TEST_BINS = \
	testmpfr_c$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS) $(TEST_BINS)
	@LD_LIBRARY_PATH=$(MPFR_LIB_DIR) ./testmpfr_c 12.12312312312312 34.334556443334533
	@echo

local_longtest:
