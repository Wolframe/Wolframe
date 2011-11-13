TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_filesystem

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

DYNAMIC_MODULE = mod_db_sqlite3.so

CPP_OBJS = \
	SQLite.o \
	SQLiteConfig.o \
	SQLiteModule.o

ifeq ($(WITH_LOCAL_SQLITE3),1)

OBJS = \
	$(TOPDIR)/sqlite3/sqlite3.o

endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/modules/database
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(libdir)/modules/database

local_test:
