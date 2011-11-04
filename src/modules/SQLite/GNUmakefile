TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include

INCLUDE_LIBS =

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

DYNAMIC_LIB = mod_db_sqlite3.so
DYNAMIC_LIB_MAJOR = 0     
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0 

OBJS =

#	database/SQLite.o \
#	database/SQLiteConfig.o \
#	database/SQLiteModule.o

ifeq ($(WITH_LOCAL_SQLITE3),1)
OBJS += \
	$(TOPDIR)/sqlite3/sqlite3.o
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/modules
	$(INSTALL) -m 0775 $(DYNAMIC_LIB) $(libdir)/modules

local_test:
