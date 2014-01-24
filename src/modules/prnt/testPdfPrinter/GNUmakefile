TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_prnt

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_prnt

DYNAMIC_MODULE = mod_test_pdf_printer.so

CPP_OBJS = \
	prnt_pdfPrinterDocument_testtrace.o \
	mod_test_pdf_printer.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

