TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR) -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

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

# libhpdf
ifeq ($(WITH_LIBHPDF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF
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
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

STATIC_LIB = libwolftest.a

DYNAMIC_LIB = libwolftest.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

STATIC_LIB_OBJS = \
	testDescription.o \
	testModules.o

CPP_OBJS = \
	testDescription.o \
	testModules.o

CPP_BINS = \
	wtestcheck$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(STATIC_LIB) $(DYNAMIC_LIB)

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
