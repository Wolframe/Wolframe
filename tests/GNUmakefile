TOPDIR = ..

SUBDIRS =

ifeq ($(WITH_EXPECT),1)
SUBDIRS += expect config
endif

ifeq ($(WITH_LIBXML2),1)
SUBDIRS += libxml2
endif

ifeq ($(WITH_LIBXSLT),1)
ifeq ($(WITH_LIBXML2),1)
SUBDIRS += libxslt
endif
endif

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	threadLeakTest$(EXE) \
	testMethodMemberPointerTemplate$(EXE) \
	testNamedRetVal$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	singletonTest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing$(EXE) \
	configValueParser$(EXE) \
	configStructParser$(EXE) \

-include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./testMethodMemberPointerTemplate
	@echo
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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./configValueParser
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./configStructParser $(TOPDIR)/src/wolframe.conf
	@echo
	@echo

local_longtest: poolTest
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./poolTest
	@echo
	@echo
