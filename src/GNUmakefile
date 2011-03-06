TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

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

# pam

ifdef WITH_PAM

INCLUDE_CPPFLAGS += \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

endif

LOCAL_STATIC_LIB = \
	libwolframe.a

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	resolvePath.o \
	commandLine.o \
	logLevel.o \
	logSyslogFacility.o \
	logBackend.o \
	configHelpers.o \
	configServer.o \
	configLogger.o \
	configDatabase.o \
	appConfig.o \
	protocol_ioblocks.o \
	acceptor.o \
	getPassword.o \
	SSLcertificateInfo.o \
	serverEndpoint.o \
	connection.o \
	server.o \
	authentication.o \
	authentication_textfile.o

# pam
ifdef WITH_PAM
LOCAL_STATIC_LIB_OBJS += \
	authentication_pam.o
endif

OBJS = \
	$(LOCAL_STATIC_LIB_OBJS) \
	appProperties.o \
	appConfigDefinition.o \
	wolframeHandler.o \
	posixMain.o

CPP_BINS = \
	wolframed$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)
	@rm -f *.sho++

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(CPP_BINS) $(bindir)

local_test:

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./wolframed -c wolframe.conf -f
