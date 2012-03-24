TOPDIR = .

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS = src gtest tests clients include docs

ifeq ($(WITH_LIBHARU),1)
SUBDIRS := haru $(SUBDIRS)
endif
ifeq ($(WITH_LUA),1)
SUBDIRS := lua $(SUBDIRS)
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
