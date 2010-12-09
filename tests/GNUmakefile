TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/examples/pecho \
	-I$(TOPDIR)/examples/xmlselect \
	-I$(BOOST_DIR)/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_DIR)/lib

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options \
	$(TOPDIR)/src/libsmerp.a \
	$(GTEST_LIBS)

TEST_OBJS = \
	$(TOPDIR)/examples/pecho/pechoHandler.o

TEST_CPP_BINS = \
	versionTest$(EXE) \
	counterTest$(EXE) \
	threadLeakTest$(EXE) \
	pechoHandlerTest$(EXE) \
	xmlselectHandlerTest$(EXE) \
	logTest$(EXE)

ifdef WITH_GTEST
TEST_CPP_BINS += \
	version-gTest$(EXE) \
	counter-gTest$(EXE) \
	pechoHandler-gTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

pechoHandlerTest$(EXE): $(TOPDIR)/src/libsmerp.a $(TOPDIR)/examples/pecho/pechoHandler.o

ifdef WITH_GTEST
pechoHandler-gTest$(EXE): $(TOPDIR)/src/libsmerp.a $(TOPDIR)/examples/pecho/pechoHandler.o
endif

xmlselectHandlerTest$(EXE): $(TOPDIR)/src/libsmerp.a $(TOPDIR)/examples/xmlselect/xmlselectHandler.o

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o

local_distclean:

local_test:
	@echo
	@echo "Testing version..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./versionTest
	@echo "Testing counter.."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./counterTest
ifdef WITH_GTEST
	@echo "Testing with Google test framework..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./version-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./counter-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./pechoHandler-gTest
endif
	@echo
	@echo "Testing boost log..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./logTest
	@echo
