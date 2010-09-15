TOPDIR = ..

SUBDIRS =

INCLUDE_CPPFLAGS = \
	-DBOOST_LOG_USE_NATIVE_SYSLOG

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/3rdparty/include

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/3rdparty/src/boost/log

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_log -lboost_log_setup -lgtest

TEST_OBJS = \
	$(TOPDIR)/src/version.o

TEST_CPP_BINS = \
	singletonTest$(EXE) \
	versionTest$(EXE) \
	version-gTest$(EXE) \
	logTest$(EXE) \
	threadLeakTest$(EXE)	

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
	@./version-gTest
	@echo
	@echo "Testing boost log..."
	@./logTest
	@echo

