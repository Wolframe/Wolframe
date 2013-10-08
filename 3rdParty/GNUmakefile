TOPDIR = ..

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS = gtest

ifeq ($(WITH_LOCAL_LIBHPDF),1)
SUBDIRS += libhpdf
endif

ifeq ($(WITH_LOCAL_FREEIMAGE),1)
SUBDIRS += freeimage
endif

ifeq ($(WITH_CJSON),1)
SUBDIRS += libcjson
endif

ifeq ($(WITH_LOCAL_SQLITE3),1)
SUBDIRS += sqlite3
endif

ifeq ($(WITH_LUA),1)
SUBDIRS += lua
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk
