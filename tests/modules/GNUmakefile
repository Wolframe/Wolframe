TOPDIR = ../..

SUBDIRS = tests authentication

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/wtest/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/loadModules.o \
	$(TOPDIR)/src/libwolframed.a \
	-lwolftest \
	-lwolframe -lwfsingleton \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system \
	$(LIBS_DL) $(LIBS_LT)

TEST_OBJS =

TEST_CPP_BINS = \
	moduleTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe"

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./moduleTest
	@echo
endif
