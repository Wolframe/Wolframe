TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem

STATIC_LIB = libwolframe.a

DYNAMIC_LIB = libwolframe.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

LOGGER_OBJS = \
	logger/logLevel.o \
	logger/logSyslogFacility.o \
	logger/logError.o \
	logger/logObject.o \
	logger/logBackendConsole.o \
	logger/logBackendFile.o \
	logger/logBackendSyslog.o \
	logger/logBackendImpl.o \
	logger/loggerConfig.o

PROVIDER_OBJS = \
	provider/Authentication.o \
	provider/Authorization.o \
	provider/Audit.o \
	provider/AAAAproviderImpl.o \
	provider/AAAAproviderConfig.o \
	provider/DBproviderImpl.o \
	provider/DBproviderConfig.o \
	provider/procProvider.o \
	provider/procProviderImpl.o

ADT_OBJS = \
	types_bcdArithmetic.o \
	types_dateArithmetic.o \
	types_doctype.o \
	types_variant.o \
	types_variantStructDescription.o \
	types_variantStruct.o

CPP_OBJS = \
	$(LOGGER_OBJS) \
	$(PROVIDER_OBJS) \
	$(ADT_OBJS) \
	version.o \
	getPassword.o \
	user.o \
	moduleDirectory.o \
	resolvePath.o \
	stringUtils.o \
	parseUtils.o \
	types_allocators.o \
	password.o \
	globalRndGen.o \
	HMAC.o \
	PBKDF2.o \
	CRAM.o

OBJS = \
	b64encode.o \
	b64decode.o \
	byte2hex.o \
	sha1.o \
	sha2.o \
	hmac_sha1.o \
	hmac_sha256.o \
	pbkdf2_hmac_sha1.o \
	md5.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
