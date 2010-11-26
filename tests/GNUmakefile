TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_DIR)/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_DIR)/lib

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(GTEST_LIBS)

TEST_OBJS = \
	$(TOPDIR)/src/version.o \
	$(TOPDIR)/src/logger.o \
	$(TOPDIR)/src/pechoHandler.o \
	$(TOPDIR)/src/xmlselectHandler.o

TEST_CPP_BINS = \
	versionTest$(EXE) \
	threadLeakTest$(EXE) \
	logTest$(EXE) \
	pechoHandlerTest$(EXE) \
	xmlselectHandlerTest$(EXE)

ifdef WITH_GTEST
TEST_CPP_BINS += \
	version-gTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -rf logTest.log

local_distclean:

local_test:
	@echo
	@echo "Testing version..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./versionTest
	@echo
ifdef WITH_GTEST
	@echo "Testing Google test.."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./version-gTest
	@echo
endif
	@echo "Testing boost log..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./logTest
	@echo
