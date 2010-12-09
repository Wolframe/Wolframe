TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_DIR)/lib

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_DIR)/include

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

STATIC_LIB = \
	libsmerp.a

OBJS = \
	version.o \
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
	smerpHandler.o \
	xmlselectHandler.o \
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
