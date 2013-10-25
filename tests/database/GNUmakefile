TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

# Postgresql
ifeq ($(WITH_PGSQL),1)
SUBDIRS += PostgreSQL
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
SUBDIRS += Oracle
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
SUBDIRS += SQLite3
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
