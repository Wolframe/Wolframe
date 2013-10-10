TOPDIR = ../../..

SUBDIRS = testPdfPrinter

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_LIBHPDF),1)
ifeq ($(WITH_TEXTWOLF),1)
SUBDIRS += haruPdfPrinter
endif
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

