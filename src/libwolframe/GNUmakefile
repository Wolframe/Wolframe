TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

# textwolf support for module types/string
ifeq ($(WITH_TEXTWOLF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_TEXTWOLF
endif

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

# textwolf support for module types/string
ifeq ($(WITH_TEXTWOLF),1)
INCLUDE_DIRS += \
	-I$(TOPDIR)/3rdParty/textwolf/include
endif

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem \

STATIC_LIB = libwolframe.a

DYNAMIC_LIB = libwolframe.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

TYP_OBJS = \
	types/conversions.o \
	types/numberBaseConversion.o \
	types/string.o \
	types/bignumber.o \
	types/datetime.o \
	types/doctype.o \
	types/customDataType.o \
	types/variant.o \
	types/variantStructDescription.o \
	types/variantStruct.o \
	types/form.o

UTIL_OBJS = \
	utils/typeSignature.o \
	utils/allocators.o \
	utils/malloc.o \
	utils/stringUtils.o \
	utils/fileUtils.o \
	utils/parseUtils.o \
	utils/printFormats.o

CPP_OBJS = \
	$(TYP_OBJS) \
	$(UTIL_OBJS) \
	logObject.o \
	logError.o \
	version.o \
	getPassword.o \
	user.o \
	HMAC.o \
	PBKDF2.o \
#	password.o \
#	CRAM.o

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
