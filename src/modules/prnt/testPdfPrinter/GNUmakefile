TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe

DYNAMIC_MODULE = mod_test_pdf_printer.so

CPP_OBJS = \
	$(TOPDIR)/src/types_bcdArithmetic.o \
	$(TOPDIR)/src/prnt_pdfPrinter.o \
	$(TOPDIR)/src/prnt_pdfPrinter_testtrace.o \
	$(TOPDIR)/src/prnt_pdfPrinterDocument_testtrace.o \
	$(TOPDIR)/src/prnt_pdfPrinterExpression.o \
	$(TOPDIR)/src/prnt_pdfPrinterMethod.o \
	$(TOPDIR)/src/prnt_pdfPrinterVariable.o \
	mod_test_pdf_printer.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

