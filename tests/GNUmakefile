TOPDIR = ..

SUBDIRS =

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(GTEST_LIBS)

# boost-log

ifdef WITH_BOOST_LOG

INCLUDE_LDFLAGS += \
	-L$(TOPDIR)/3rdparty/src/boost/log

INCLUDE_DIRS += \
	-I$(TOPDIR)/3rdparty/include

INCLUDE_LIBS += \
	-lboost_log -lboost_log_setup

endif

TEST_OBJS = \
	$(TOPDIR)/src/version.o

TEST_CPP_BINS = \
	singletonTest$(EXE) \
	versionTest$(EXE) \
	threadLeakTest$(EXE)

ifdef WITH_GTEST
TEST_CPP_BINS += \
	version-gTest$(EXE)
endif

ifdef WITH_BOOST_LOG
TEST_CPP_BINS += logTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

logTest$(EXE): logTest.o \
	$(TOPDIR)/3rdparty/src/boost/log/libboost_log.a \
	$(TOPDIR)/3rdparty/src/boost/log/libboost_log_setup.a

local_all:

local_clean:
	-rm -rf logTest.log

local_distclean:

local_test:
	@echo
	@echo "Testing singleton template..."
	@./singletonTest
	@echo
	@echo "Testing version..."
	@./versionTest
	@echo
ifdef WITH_GTEST
	@echo "Testing Google test.."
	@./version-gTest
	@echo
endif
ifdef WITH_BOOST_LOG
	@echo "Testing boost log..."
	@./logTest
	@echo
endif

