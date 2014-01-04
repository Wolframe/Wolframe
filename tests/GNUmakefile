TOPDIR = ..

SUBDIRS = 3rdParty modules database AAAA simpleFormCompiler wolfilter

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_EXPECT),1)
ifneq "$(PLATFORM)" "FREEBSD"
ifneq "$(PLATFORM)" "NETBSD"
ifneq "$(PLATFORM)" "SUNOS"
SUBDIRS += expect config
endif
endif
endif
endif

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libwolframe_prnt \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframe.a \
	-lwolftest \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lwolframe_prnt \
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
	-I$(TOPDIR)/3rdParty/lua/src

INCLUDE_LIBS += \
	$(TOPDIR)/3rdParty/lua/src/liblua.a \
	$(LIBS_DL)
endif

# libhpdf
ifeq ($(WITH_LIBHPDF),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

# postgres
ifeq ($(WITH_PGSQL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PGSQL
INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(PGSQL_LIB_DIRS)
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
INCLUDE_CXXFLAGS += \
	-DWITH_ORACLE
INCLUDE_DIRS += \
	$(ORACLE_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(ORACLE_LIB_DIRS)
INCLUDE_LIBS += \
	$(ORACLE_LIBS)
endif

# Python (missing symbols on FreeBSD only, should
# be investigated as we want to keep all python
# dependencies strictly to the modules), otherwise
# we also have to link 'wolframed' against -lpython
# which is not acceptable. Keep the hackaround for
# now to make FreeBSD happy..
ifeq "$(PLATFORM)" "FREEBSD"
ifeq ($(WITH_PYTHON),1)
INCLUDE_LDFLAGS += \
	$(PYTHON_LDFLAGS)
INCLUDE_LIBS += \
	$(PYTHON_LIBS)
endif
endif

TEST_OBJS =

TEST_CPP_BINS = \
	testMethodMemberPointerTemplate$(EXE) \
	testNamedRetVal$(EXE) \
	testVirtualInheritance$(EXE) \
	testArrayDoublingAllocator$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	testBoostPath$(EXE) \
	singletonTest$(EXE) \
	byte2hexTest$(EXE) \
	base64Test$(EXE) \
	base64Utility$(EXE) \
	testSHA$(EXE) \
	testMD5$(EXE) \
	CRAMtest$(EXE) \
	HMACtest$(EXE) \
	PBKDF2test$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing$(EXE) \
	testProtocolLFdotEscaping$(EXE) \
	testVariantStruct$(EXE) \
	configValueParser$(EXE) \
	configStructParser$(EXE) \
	testRand$(EXE) \
	testTimestamp$(EXE) \
	testWolfilter$(EXE) \
	testVariantTypes$(EXE)
# DISABLED (moved to .Attic) unitLibraryTest$(EXE)

#	stringNormalizationTest$(EXE) \
# DISABLED (only for memory corruption checks)	testTestAllocator$(EXE) \

-include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a
testSHA$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
testMD5$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
byte2hexTest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
CRAMtest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
HMACtest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
PBKDF2test$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
protocolArgumentParsing$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testProtocolLFdotEscaping$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testVariantStruct$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
configValueParser$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
configStructParser$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testWolfilter$(EXE): $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a
testArrayDoublingAllocator$(EXE): $(TOPDIR)/src/libwolframe.a
testTestAllocator$(EXE): $(TOPDIR)/src/libwolframe.a
stringNormalizationTest$(EXE): $(TOPDIR)/src/libwolframe_functions/libwolframe_functions.a
testVariantTypes$(EXE):  $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log
	-rm -rf temp

local_distclean:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_functions:$(TOPDIR)/src/libwolframe_prnt:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/wtest"

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_ORACLE),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(ORACLE_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

ifeq ($(WITH_PYTHON),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(PYTHON_LIB_DIR)"
endif

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@mkdir -p temp
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testMethodMemberPointerTemplate
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testNamedRetVal
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testVirtualInheritance
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./logTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./versionTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./counterTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./resolvePathTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testBoostPath
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./singletonTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testSHA
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testMD5
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./byte2hexTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./base64Test
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testRand
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testTimestamp
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./CRAMtest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./HMACtest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./protocolArgumentParsing
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./configValueParser
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./configStructParser config/structparser.conf
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testProtocolLFdotEscaping
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testVariantStruct
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testVariantTypes
	@echo
#	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./stringNormalizationTest
#	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testWolfilter
	@echo
## DISABLED (moved to .Attic) @LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./unitLibraryTest
	@echo
	@echo
endif

local_longtest: poolTest
ifeq ($(RUN_TESTS),0)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./poolTest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testArrayDoublingAllocator
	@echo
# DISABLED (only for memory corruption checks):
#	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testTestAllocator
#	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./PBKDF2test
	@echo
	@echo
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
