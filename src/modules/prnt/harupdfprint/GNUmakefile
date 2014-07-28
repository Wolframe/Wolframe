TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/3rdParty/textwolf/include \
	-I$(TOPDIR)/src \
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

NOINST_STATIC_LIB = mod_print_harupdf_lib.a

DYNAMIC_MODULE = mod_print_harupdf.so

LIBHPDF_OBJS =

CPP_OBJS = \
	pdfPrinter.o \
	pdfPrinterExpression.o \
	pdfPrinterMethod.o \
	pdfPrinterVariable.o \
	pdfPrintProgramType.o \
	pdfPrinterDocumentImpl.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

