TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS += directmap

ifeq ($(WITH_LUA),1)
SUBDIRS += lua
endif

ifeq ($(WITH_PYTHON),1)
SUBDIRS += python
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

