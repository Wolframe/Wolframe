TOPDIR = ..

SUBDIRS = lib cmdline

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_QT),1)
SUBDIRS += qt
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

cmdline: lib

local_all:

local_test:

local_clean:

local_distclean:
