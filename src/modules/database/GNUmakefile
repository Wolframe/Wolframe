TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

# Postgresql
ifeq ($(WITH_PGSQL),1)
SUBDIRS += postgresql
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
SUBDIRS += sqlite3
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
SUBDIRS += oracle
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
