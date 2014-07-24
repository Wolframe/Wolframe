TOPDIR = ../../../..

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS = testtrace

# Postgresql
ifeq ($(WITH_PGSQL),1)
SUBDIRS += postgresql
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
SUBDIRS += oracle
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
SUBDIRS += sqlite3 sqlite3simple
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

local_install:

local_uninstall:
