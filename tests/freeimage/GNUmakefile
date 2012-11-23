TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/gtest/include \
	$(FREEIMAGE_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(FREEIMAGE_LIB_DIRS)
	
INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	$(FREEIMAGE_LIBS)
	
TEST_OBJS =

TEST_CPP_BINS = \
	freeimageTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(FREEIMAGE_LIB_DIR) ./freeimageTest
	@echo
endif
