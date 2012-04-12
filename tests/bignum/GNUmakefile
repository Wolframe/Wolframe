TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_LIBGMP),1)
SUBDIRS += libgmp
endif

ifeq ($(WITH_MPFR),1)
SUBDIRS += mpfr
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk
