TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_filesystem

DYNAMIC_MODULE = mod_auth_textfile.so

CPP_OBJS = \
	TextFileAuthConfig.o \
	TextFileAuth.o \
	TextFileAuthModule.o \
	../../../resolvePath.o \
	../../../logger/logLevel.o \
	../../../logger/logSyslogFacility.o \
	../../../logger/logComponent.o \
	../../../logger/logError.o \
	../../../logger/logBackendConsole.o \
	../../../logger/logBackendFile.o \
	../../../logger/logBackendSyslog.o \
	../../../logger/logBackendImpl.o \
	../../../logger/loggerConfig.o \
	../../../logger/logger.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/modules/authentication
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(libdir)/modules/authentication

local_test:

