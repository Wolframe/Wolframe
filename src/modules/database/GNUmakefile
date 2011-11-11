TOPDIR = ../../..

SUBDIRS =

# Postgresql
ifeq ($(WITH_PGSQL),1)
SUBDIRS += postgresql
endif

# sqlite3
ifeq ($(WITH_SQLITE3),1)
SUBDIRS += sqlite3
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
