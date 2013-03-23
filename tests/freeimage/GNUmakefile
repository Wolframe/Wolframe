TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

# for static linking set -DFREEIMAGE_LIB and use
# $(TOPDIR)/freeimage/libfreeimage.a directly
INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/gtest/include \
	$(FREEIMAGE_INCLUDE_DIRS) \
	$(FREEIMAGEPLUS_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(FREEIMAGE_LIB_DIRS) \
	$(FREEIMAGEPLUS_LIB_DIRS)
	
INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	$(FREEIMAGEPLUS_LIBS) \
	$(FREEIMAGE_LIBS)

TEST_OBJS =

TEST_CPP_BINS = \
	freeimageTest$(EXE) \
	freeimageTestCpp$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -f thumb.png thumbcpp.png testmem.png

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(FREEIMAGE_LIB_DIR):$(FREEIMAGEPLUS_LIB_DIR) ./freeimageTest
	@LD_LIBRARY_PATH=$(FREEIMAGE_LIB_DIR):$(FREEIMAGEPLUS_LIB_DIR) ./freeimageTestCpp
	@echo
endif
