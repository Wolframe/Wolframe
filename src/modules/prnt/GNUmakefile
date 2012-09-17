TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS += testPdfPrinter

ifeq ($(WITH_LIBHPDF),1)
#PF:DISABLED: SUBDIRS += haruPdfPrinter
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

