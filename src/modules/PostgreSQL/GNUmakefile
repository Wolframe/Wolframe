TOPDIR = ../../..

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

ifeq ($(WITH_SSL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif


# Postgresql

ifeq ($(WITH_PGSQL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PGSQL

INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

LOCAL_STATIC_LIB = \
	libwolframe.a

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
	logger/logger.o

DB_OBJS =

ifeq ($(WITH_PGSQL),1)
DB_OBJS += \
	database/PostgreSQLconfig.o \
	database/PostgreSQL.o \
	database/PostgreSQLmodule.o
endif

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	resolvePath.o \
	commandLine.o \
	$(LOGGER_OBJS) \
	serviceConfig.o \
	serviceBanner.o \
	database/databaseConfig.o \
	appConfig.o \
	config_descriptionBase.o \
	referenceConfig.o \
	protocol_ioblocks.o \
	acceptor.o \
	getPassword.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o \
	moduleLoaderConfig.o \
	processor/procProvider.o \
	processor/echoProcessor.o \
	processor/echoProcessorModule.o \
	$(DB_OBJS) \
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
	moduleImpersonation.o \
	wolframeHandler.o \
	database/DBprovider.o \
	posixMain.o
# Aba: link trouble in dbTest!
# also suggesting with_sqlite flags and DB_OBJS..
#	database/SQLite.o
#	database/SQLiteConfig.o
#	database/PostgreSQL.o \
#	database/PostgreSQLconfig.o \

CPP_BINS = \
#	postgresql$(EXE)

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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./wolframed -c wolframe.conf -f
