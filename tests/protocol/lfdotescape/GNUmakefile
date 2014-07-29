TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/src/modules/protocol/standard \
	-I$(TOPDIR)/wtest/include \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	-lwfsingleton \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_program_options \
	$(LIBS_LT) \
	$(LIBS_DL) $(LIBS_NET)

CPP_OBJS = \
	protocolLFdotEscapingFiles.o

TEST_CPP_BINS = \
	protocolLFdotEscaping$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

protocolLFdotEscaping$(EXE): protocolLFdotEscaping.o protocolLFdotEscapingFiles.o $(TOPDIR)/src/libwolframed.a

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/wtest"

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:$(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./protocolLFdotEscaping
	@echo
endif

