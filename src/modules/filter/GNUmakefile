TOPDIR = ../../..

SUBDIRS = blob

-include $(TOPDIR)/makefiles/gmake/platform.mk

# textwolf
ifeq ($(WITH_TEXTWOLF),1)
SUBDIRS += textwolf token char line
endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
SUBDIRS += libxml2
endif

# rapidjson
ifeq ($(WITH_CJSON),1)
SUBDIRS += cjson
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

