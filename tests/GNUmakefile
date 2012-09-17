TOPDIR = ..

SUBDIRS = modules AAAA simpleFormCompiler wolfilter

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

ifeq ($(WITH_LIBHPDF),1)
SUBDIRS += libhpdf
endif

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include \
	-I$(TOPDIR)/wtest

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/wtest/wolftest.a \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwolframe_serialize/libwolframe_serialize.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_program_options

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)
endif

# lua
ifeq ($(WITH_LUA),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS += \
	-I$(TOPDIR)/lua/src

INCLUDE_LIBS += \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)
endif

# libhpdf
ifeq ($(WITH_LIBHPDF),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	threadLeakTest$(EXE) \
	testMethodMemberPointerTemplate$(EXE) \
	testNamedRetVal$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	testBoostPath$(EXE) \
	singletonTest$(EXE) \
	byte2hexTest$(EXE) \
	base64Test$(EXE) \
	base64Utility$(EXE) \
	testSHA2$(EXE) \
	CRAMtest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing$(EXE) \
	testProtocolLFdotEscaping$(EXE) \
	configValueParser$(EXE) \
	configStructParser$(EXE) \
	testRand$(EXE) \
	testTimestamp$(EXE) \
	testWolfilter$(EXE)

#PF:DISABLED TEMPORARILY: testDatabaseProcessorVM$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a
testSHA2$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
byte2hexTest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
CRAMtest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
protocolArgumentParsing$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testProtocolLFdotEscaping$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
configValueParser$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
configStructParser$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testWolfilter$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
#PF:DISABLED TEMPORARILY: testDatabaseProcessorVM$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log
	-rm -rf temp

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@mkdir -p temp
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testMethodMemberPointerTemplate
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testNamedRetVal
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./logTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./versionTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./counterTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./resolvePathTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testBoostPath
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./singletonTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testSHA2
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./byte2hexTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./base64Test
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testRand
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testTimestamp
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./CRAMtest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./protocolArgumentParsing
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./configValueParser
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./configStructParser config/structparser.conf
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testProtocolLFdotEscaping
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize ./testWolfilter
	@echo
#PF:DISABLED TEMPORARILY: 	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./testDatabaseProcessorVM
	@echo
	@echo
endif

local_longtest: poolTest
ifeq ($(RUN_TESTS),0)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./poolTest
	@echo
	@echo
endif
