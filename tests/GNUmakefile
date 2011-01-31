TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/examples/pecho \
	-I$(TOPDIR)/examples/xmlselect \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

ifdef WITH_GTEST
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
	$(TOPDIR)/src/libsmerp.a \
	$(GTEST_LIBS)

# openssl

ifdef WITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

TEST_OBJS =
#	$(TOPDIR)/examples/pecho/pechoHandler.o
#	$(TOPDIR)/examples/xmlselect/xmlselectHandler.o

TEST_CPP_BINS = \
	threadLeakTest$(EXE)

ifdef WITH_GTEST
TEST_CPP_BINS += \
	versionTest$(EXE) \
	counterTest$(EXE) \
	resolvePathTest$(EXE) \
	poolTest$(EXE) \
	logTest$(EXE) \
	protocolArgumentParsing-gTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

ifdef WITH_GTEST
pechoHandler-gTest$(EXE): pechoHandler-gTest.o $(TOPDIR)/src/libsmerp.a $(TOPDIR)/examples/pecho/pechoHandler.o
	$(CCPP_LINK) -o $@ $(LDFLAGS) $(OBJS) $< $(LIBS) $(TOPDIR)/examples/pecho/pechoHandler.o
endif

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f logTest.log

local_distclean:

local_test:
ifdef WITH_GTEST
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./logTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./versionTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./counterTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./resolvePathTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./poolTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./protocolArgumentParsing-gTest
endif
	@echo
	@echo
