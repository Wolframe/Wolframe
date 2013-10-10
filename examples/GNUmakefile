TOPDIR = ..

SUBDIRS = echo pecho tproc

ifeq ($(WITH_TEXTWOLF),1)
SUBDIRS += iproc
endif
ifeq ($(WITH_LUA),1)
SUBDIRS += lua
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
