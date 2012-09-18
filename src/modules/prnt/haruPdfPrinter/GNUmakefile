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

# libhpdf
INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)


DYNAMIC_MODULE = mod_haru_pdf_printer.so

LIBHPDF_OBJS =

CPP_OBJS = \
	$(TOPDIR)/src/types_bcdArithmetic.o \
	$(TOPDIR)/src/prnt_pdfPrinter.o \
	$(TOPDIR)/src/prnt_pdfPrinter_libhpdf.o \
	$(TOPDIR)/src/prnt_pdfPrinterDocument_libhpdf.o \
	$(TOPDIR)/src/prnt_pdfPrinterExpression.o \
	$(TOPDIR)/src/prnt_pdfPrinterMethod.o \
	$(TOPDIR)/src/prnt_pdfPrinterVariable.o \
	mod_haru_pdf_printer.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

