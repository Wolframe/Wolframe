TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBGMP_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(LIBGMP_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBGMP_LIBS) $(LIBGMP_CXX_LIBS)

TEST_BINS = \
	testlibgmp_c$(EXE) \
	testlibgmp_cpp$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS) $(TEST_BINS)
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testlibgmp_c 4 5
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testlibgmp_c 424234238490238409238402380482304 52342342342342342342342323
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testlibgmp_cpp 4 5
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testlibgmp_cpp 424234238490238409238402380482304 52342342342342342342342323
	@echo

local_longtest:
