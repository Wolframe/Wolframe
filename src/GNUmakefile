TOPDIR = ..

SUBDIRS =

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS = \

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem


# openssl

ifdef WITH_SSL

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# boost-log

ifndef WITH_BOOST_LOG

INCLUDE_CPPFLAGS += -DFAKE_LOGGER

else

INCLUDE_LDFLAGS += \
	-L$(TOPDIR)/3rdparty/src/boost/log

INCLUDE_DIRS += \
	-I$(TOPDIR)/3rdparty/include

INCLUDE_LIBS += \
	-lboost_log -lboost_log_setup

endif


CPP_BINS = \
	smerpd$(EXE)

STATIC_LIB = \

STATIC_LIB_OBJS = \

C_PORT_OBJS = \

OBJS = \
	$(C_PORT_OBJS) \
	$(STATIC_LIB_OBJS) \
	version.o \
	appInstance.o \
	commandLine.o \
	logLevel.o \
	logSyslogFacility.o \
	logger.o \
	configFile.o \
	appConfig.o \
	acceptor.o \
	getPassword.o \
	connection.o \
	server.o \
	echoHandler.o \
	posixMain.o


CPP_BINS = \
	smerpd$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

$(STATIC_LIBS): $(STATIC_LIBS_OBJS)
	ar rcs $(STATIC_LIBS) $(STATIC_LIB_OBJS)

local_clean:
	@rm -f $(STATIC_LIBS)

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(CPP_BINS) $(bindir)
