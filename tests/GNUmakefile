TOPDIR = ..

SUBDIRS = database AAAA

ifeq ($(WITH_EXPECT),1)
SUBDIRS += expect config
endif

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframe.a

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	threadLeakTest$(EXE) \
	testNamedRetVal$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	singletonTest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing$(EXE) \
	configStructParser$(EXE) \
	configValueParser$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log

local_distclean:

local_test:
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./testNamedRetVal
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./logTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./versionTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./counterTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./resolvePathTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./singletonTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./protocolArgumentParsing
	@echo
	@echo

local_longtest:
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./poolTest
	@echo
	@echo
