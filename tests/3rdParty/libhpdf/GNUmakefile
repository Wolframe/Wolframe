TOPDIR = ../../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBHPDF_LIBS)

TEST_CPP_BINS = \
	testlibhpdf$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -f *.pdf test2.raw

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR):$(LIBPNG_LIB_DIR) ./testlibhpdf
	@echo

local_longtest:
