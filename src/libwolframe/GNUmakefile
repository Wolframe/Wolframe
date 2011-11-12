TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lboost_filesystem

STATIC_LIB = libwolframe.a

DYNAMIC_LIB = libwolframe.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

LOGGER_OBJS = \
	logger/logLevel.o \
	logger/logSyslogFacility.o \
	logger/logComponent.o \
	logger/logError.o \
	logger/logBackendConsole.o \
	logger/logBackendFile.o \
	logger/logBackendSyslog.o \
	logger/logBackendImpl.o \
	logger/loggerConfig.o \
	logger/logger.o \

CPP_OBJS = \
	$(LOGGER_OBJS) \
	resolvePath.o \
	referenceConfig.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
