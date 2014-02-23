TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_PYTHON),1)
SUBDIRS += python
endif

ifeq ($(WITH_LIBXML2),1)
SUBDIRS += libxml2
endif

ifeq ($(WITH_LIBXSLT),1)
SUBDIRS += libxslt
endif

ifeq ($(WITH_LIBHPDF),1)
SUBDIRS += libhpdf
endif

ifeq ($(WITH_ICU),1)
ifneq "$(PLATFORM)" "NETBSD"
SUBDIRS += boost-locale
endif
endif

ifeq ($(WITH_FREEIMAGE),1)
SUBDIRS += freeimage
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk
