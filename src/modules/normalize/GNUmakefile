TOPDIR = ../../..

SUBDIRS = number base64

-include $(TOPDIR)/makefiles/gmake/platform.mk

# textwolf
ifeq ($(WITH_TEXTWOLF),1)
SUBDIRS += string
endif

ifeq ($(WITH_ICU),1)
SUBDIRS += locale
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

