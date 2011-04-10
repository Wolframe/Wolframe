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

# sqlite3

ifeq ($(WITH_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

# Postgresql

ifeq ($(WITH_PGSQL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PGSQL

INCLUDE_DIRS += \
	-I$(PGSQL_INCLUDE_DIR)

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
	logger/logger.o

AAAA_OBJS = \
	AAAA/authentication.o \
	AAAA/authentication_textfile.o

ifeq ($(WITH_PAM),1)
AAAA_OBJS += \
	AAAA/authentication_pam.o
endif

ifeq ($(WITH_SQLITE3),1)
AAAA_OBJS += \
	AAAA/authentication_db_sqlite3.o
endif

DB_OBJS =

# Sqlite3

ifeq ($(WITH_SQLITE3),1)
DB_OBJS += \
	db/sdsqlite/sdsqlite.o
endif

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	resolvePath.o \
	commandLine.o \
	$(LOGGER_OBJS) \
	configHelpers.o \
	serviceConfig.o \
	serviceBanner.o \
	loggerConfig.o \
	databaseConfig.o \
	appConfig.o \
	protocol_ioblocks.o \
	acceptor.o \
	getPassword.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o \
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
	database.o \
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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./wolframed -c wolframe.conf -f
