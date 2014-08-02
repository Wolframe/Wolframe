TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/src/modules/protocol/wolframe \
	-I$(TOPDIR)/wtest/include \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_langbind \
	-lwolframe_serialize \
	-lwolframe_database \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_program_options \
	$(LIBS_LT) \
	$(LIBS_DL) $(LIBS_NET)

CPP_OBJS = \
	testProtocolFiles.o

TEST_CPP_BINS = \
	testProtocol$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

testProtocol$(EXE): $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a $(TOPDIR)/src/libwolframe_database/libwolframe_database.a

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/wtest"

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:$(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testProtocol
	@echo
endif

