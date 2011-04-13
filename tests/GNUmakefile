TOPDIR = ..

SUBDIRS =

ifeq ($(WITH_EXPECT),1)
SUBDIRS += expect config
endif

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/examples/pecho \
	-I$(TOPDIR)/examples/mtproc \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

ifeq ($(WITH_GTEST),1)
INCLUDE_CPPFLAGS += \
	-I$(GTEST_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(GTEST_LIB_DIR)
endif

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options \
	$(TOPDIR)/src/libwolframe.a

ifeq ($(WITH_GTEST),1)
INCLUDE_LIBS += \
	$(GTEST_LIBS)
endif

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# pam

ifeq ($(WITH_PAM),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

endif

# sasl

ifeq ($(WITH_SASL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SASL

INCLUDE_DIRS += \
	-I$(SASL_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS += \
	$(SASL_LIBS)

endif

# sqlite3

ifeq ($(WITH_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	threadLeakTest$(EXE) \
	authTest$(EXE)

ifeq ($(WITH_GTEST),1)
TEST_CPP_BINS += \
	testNamedRetVal$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	configHelpersTest$(EXE) \
	singletonTest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing-gTest$(EXE) \
	configValueParser-gTest$(EXE) \
	ptreeGenerator-gTest$(EXE) \
	pechoHandler-gTest$(EXE) \
	mtprocHandler-gTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a
authTest$(EXE): $(TOPDIR)/src/libwolframe.a

ifeq ($(WITH_GTEST),1)
PECHO_GTEST_OBJS=$(TOPDIR)/examples/pecho/pechoHandler.o
pechoHandler-gTest$(EXE): pechoHandler-gTest.o $(PECHO_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) $(PECHO_GTEST_OBJS) $(OBJS) $< $(LIBS)

MTPROC_GTEST_OBJS = \
	$(TOPDIR)/examples/mtproc/mtprocHandler.o \
	$(TOPDIR)/examples/mtproc/implementation.o \
	$(TOPDIR)/examples/mtproc/dispatcher.o
mtprocHandler-gTest$(EXE): mtprocHandler-gTest.o $(MTPROC_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) $(MTPROC_GTEST_OBJS) $(OBJS) $< $(LIBS)

ptreeGenerator-gTest$(EXE): ptreeGenerator-gTest.o  $(TOPDIR)/examples/mtproc/generators/propertyTree.o $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS)  $(TOPDIR)/examples/mtproc/generators/propertyTree.o $(OBJS) $< $(LIBS)
endif

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log

local_distclean:

local_test:
ifeq ($(WITH_GTEST),1)
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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./configHelpersTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./singletonTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./poolTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./protocolArgumentParsing-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./pechoHandler-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./mtprocHandler-gTest
endif
	@echo
	@echo
