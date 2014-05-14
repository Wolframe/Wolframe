TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

# for static linking set -DFREEIMAGE_LIB and use
# $(TOPDIR)/freeimage/libfreeimage.a directly
INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include \
	$(FREEIMAGE_INCLUDE_DIRS) \
	$(FREEIMAGEPLUS_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/wtest \
	$(FREEIMAGE_LIB_DIRS) \
	$(FREEIMAGEPLUS_LIB_DIRS)
	
INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	-lwolftest \
	-lboost_filesystem -lboost_system \
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

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(FREEIMAGE_LIB_DIR):$(FREEIMAGEPLUS_LIB_DIR)"

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./freeimageTest
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./freeimageTestCpp
	@echo
endif
