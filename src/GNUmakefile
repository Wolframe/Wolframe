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

ifeq ($(WITH_SSL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# pam

ifeq ($(WITH_PAM),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

endif

# sasl

ifeq ($(WITH_SASL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SASL

INCLUDE_DIRS += \
	-I$(SASL_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS += \
	$(SASL_LIBS)

endif

# sqlite3

ifeq ($(WITH_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

ifeq ($(WITH_LOCAL_SQLITE3),0)

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

ifeq ($(WITH_LOCAL_SQLITE3),1)

INCLUDE_DIRS += \
	-I$(TOPDIR)/sqlite3
endif

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

AAAA_OBJS = \
	AAAA/authentication_textfile.o \
	AAAA/AAAAprovider.o \
	AAAA/AAAAproviderConfig.o \
	AAAA/DBauthentication.o \
	AAAA/DBauthenticationConfig.o \
	AAAA/TextFileAuthentication.o \
	AAAA/TextAuthenticationConfig.o \
	AAAA/DBaudit.o \
	AAAA/DBauditConfig.o \
	AAAA/FileAudit.o \
	AAAA/FileAuditConfig.o

ifeq ($(WITH_PAM),1)
AAAA_OBJS += \
	AAAA/authentication_pam.o
endif

ifeq ($(WITH_SASL),1)
AAAA_OBJS += \
	AAAA/authentication_sasl.o
endif

ifeq ($(WITH_SQLITE3),1)
DB_OBJS += \
	database/SQLite.o \
	database/SQLiteConfig.o

ifeq ($(WITH_LOCAL_SQLITE3),1)
DB_OBJS += \
	$(TOPDIR)/sqlite3/sqlite3.o
endif

AAAA_OBJS += \
	AAAA/authentication_db_sqlite3.o
endif

ifeq ($(WITH_PGSQL),1)
DB_OBJS += \
	database/PostgreSQL.o \
	database/PostgreSQLconfig.o
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
	$(DB_OBJS) \
	$(AAAA_OBJS)

#SSL
ifeq ($(WITH_SSL),1)
LOCAL_STATIC_LIB_OBJS += \
	SSLcertificateInfo.o
endif

OBJS = \
	$(LOCAL_STATIC_LIB_OBJS) \
	appProperties.o \
	appConfigDefinition.o \
	wolframeHandler.o \
	database/DBprovider.o \
	database/DBreference.o \
	database/DBreferenceConfig.o \
	posixMain.o
# Aba: link trouble in dbTest!
# also suggesting with_sqlite flags and DB_OBJS..
#	database/SQLite.o
#	database/SQLiteConfig.o
#	database/PostgreSQL.o \
#	database/PostgreSQLconfig.o \

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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./wolframed -c wolframe.conf -f
