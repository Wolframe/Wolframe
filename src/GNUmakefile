TOPDIR = ..

SUBDIRS =

INCLUDE_CPPFLAGS = \
	-DBOOST_LOG_USE_NATIVE_SYSLOG -DWITH_SSL

ifndef WITH_BOOST_LOG
INCLUDE_CPPFLAGS += -DFAKE_LOGGER
else
ifeq "$(WITH_BOOST_LOG)" "1"
INCLUDE_CPPFLAGS += -DFAKE_LOGGER
endif
endif

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/3rdparty/src/boost/log

INCLUDE_DIRS = -I. \
	-I$(TOPDIR)/include -I$(TOPDIR)/3rdparty/include

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(OPENSSL_LIBS) \
	$(BOOST_LOG_LIBS)

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
