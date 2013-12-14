TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR) -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-Iinclude

INCLUDE_LIBS = \
	-lboost_filesystem -lboost_system

# openssl
ifeq ($(WITH_SSL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_SSL
endif

# lua
ifeq ($(WITH_LUA),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LUA
endif

# Python
ifeq ($(WITH_PYTHON),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PYTHON
endif

# libhpdf
ifeq ($(WITH_LIBHPDF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF
endif

# textwolf
ifeq ($(WITH_TEXTWOLF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_TEXTWOLF
endif

# cJSON
ifeq ($(WITH_CJSON),1)
INCLUDE_CXXFLAGS += \
	-DWITH_CJSON
endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2
endif

# libxslt
ifeq ($(WITH_LIBXSLT),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXSLT
endif

# icu
ifeq ($(WITH_ICU),1)
INCLUDE_CXXFLAGS += \
	-DWITH_ICU
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
INCLUDE_CXXFLAGS += \
	-DWITH_SQLITE3
endif

# postgres sql
ifeq ($(WITH_PGSQL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PGSQL
INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	-L$(PGSQL_LIB_DIR)
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
INCLUDE_CXXFLAGS += \
	-DWITH_ORACLE
INCLUDE_DIRS += \
	$(ORACLE_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(ORACLE_LIB_DIRS)
INCLUDE_LIBS += \
	$(ORACLE_LIBS)
endif


NOINST_STATIC_LIB = libwolftest.a

CPP_OBJS = \
	testDescription.o \
	testModules.o

CPP_BINS = \
	wtestcheck$(EXE) \
	cleanInput$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
