TOPDIR = ..

SUBDIRS = AAAA

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

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options \
	$(TOPDIR)/src/libwolframe.a

# gtest

ifeq ($(WITH_GTEST),1)
INCLUDE_CPPFLAGS += \
	-I$(GTEST_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(GTEST_LIB_DIR)

INCLUDE_LIBS += \
	$(GTEST_LIBS)
endif

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	threadLeakTest$(EXE)

ifeq ($(WITH_GTEST),1)
TEST_CPP_BINS += \
	testNamedRetVal$(EXE) \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	singletonTest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing-gTest$(EXE) \
	configValueParser-gTest$(EXE) \
	pechoHandler-gTest$(EXE) \
	mtprocHandler-gTest$(EXE)
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): $(TOPDIR)/src/libwolframe.a

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
