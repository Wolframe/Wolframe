TOPDIR = ..

PRESUBDIRS = libwolframe

SUBDIRS = modules

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(LIBS_DL)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

LOCAL_STATIC_LIB = \
	libwolframe.a

AAAA_OBJS = \
	AAAA/AAAAprovider.o \
	AAAA/AAAAproviderConfig.o

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	commandLine.o \
	serviceConfig.o \
	serviceBanner.o \
	database/databaseConfig.o \
	appConfig.o \
	config_descriptionBase.o \
	protocol_ioblocks.o \
	acceptor.o \
	getPassword.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o \
	procProvider.o \
	$(AAAA_OBJS)

#SSL
ifeq ($(WITH_SSL),1)
LOCAL_STATIC_LIB_OBJS += \
	SSLcertificateInfo.o
endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	$(LOCAL_STATIC_LIB_OBJS) \
	moduleDirectory.o \
	loadModules.o \
	wolframeHandler.o \
	database/DBprovider.o \
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
	$(INSTALL) -d -m 0755 $(sbindir)
	$(INSTALL) -m 0775 $(CPP_BINS) $(sbindir)
	$(INSTALL) -d -m 0755 $(sysconfdir)/wolframe
	$(INSTALL) -m 0755 wolframe.conf $(sysconfdir)/wolframe

local_test:

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./wolframed -c wolframe.conf -f
