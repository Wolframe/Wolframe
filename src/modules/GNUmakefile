TOPDIR = ../..

SUBDIRS = audit authentication processor

# Postgresql
ifeq ($(WITH_PGSQL),1)
SUBDIRS += PostgreSQL
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
SUBDIRS += SQLite
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
