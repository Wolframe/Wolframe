TOPDIR = ..

SUBDIRS =

INCLUDE_CPPFLAGS = \
	-DBOOST_LOG_USE_NATIVE_SYSLOG

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/libs/boost/log

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_log -lboost_log_setup

TEST_OBJS = \
	$(TOPDIR)/src/version.o

TEST_CPP_BINS = \
	singletonTest$(EXE) \
	versionTest$(EXE) \
	logTest$(EXE)

include $(TOPDIR)/makefiles/gmake/sub.mk

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
	@echo "Testing boost log..."
	@./logTest
	@echo

