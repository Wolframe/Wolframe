TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/wtest \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	-lwolftest \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwolframe_serialize/libwolframe_serialize.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_program_options

CPP_OBJS = \
	normalizeFunctions.o \
	simpleFormCompiler.o

TEST_CPP_BINS = \
	testSimpleFormCompiler$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

testSimpleFormCompiler$(EXE): $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a

local_all:

local_clean:
	@-rm -f gtestReport.txt

local_distclean:

local_install:

local_uninstall:

local_test:$(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testSimpleFormCompiler
	@echo
endif

