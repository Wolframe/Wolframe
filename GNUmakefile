TOPDIR = .

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS = src gtest wtest tests clients include docs

ifeq ($(WITH_LOCAL_LIBHPDF),1)
SUBDIRS := libhpdf $(SUBDIRS)
endif

ifeq ($(WITH_CJSON),1)
SUBDIRS := libcjson $(SUBDIRS)
endif

ifeq ($(WITH_LOCAL_SQLITE3),1)
SUBDIRS := sqlite3 $(SUBDIRS)
endif

ifeq ($(WITH_CJSON),1)
SUBDIRS := cjson $(SUBDIRS)
endif

ifeq ($(WITH_LUA),1)
SUBDIRS := lua $(SUBDIRS)
endif

ifeq ($(WITH_LOCAL_FREEIMAGE),1)
SUBDIRS := freeimage $(SUBDIRS)
endif

ifeq ($(WITH_EXAMPLES),1)
SUBDIRS += examples
endif

PACKAGE_NAME = wolframe
PACKAGE_VERSION = 0.0.1

-include $(TOPDIR)/makefiles/gmake/top.mk

ifeq ($(WITH_EXAMPLES),1)
examples: src
tests: src
endif
