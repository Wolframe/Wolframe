TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe \

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system

STATIC_LIB =

DYNAMIC_LIB = libwfshared.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

LOGGER_OBJS = \
	logger/logLevel.o \
	logger/logSyslogFacility.o \
	logger/logBackendConsole.o \
	logger/logBackendFile.o \
	logger/logBackendSyslog.o \
	logger/logBackendImpl.o

CPP_OBJS = \
	$(LOGGER_OBJS) \
	appInfo.o \
	globalRndGen.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
