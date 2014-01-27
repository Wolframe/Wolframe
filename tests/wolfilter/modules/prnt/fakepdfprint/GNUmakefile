TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(TOPDIR)/3rdParty/textwolf/include \
	-I$(TOPDIR)/src/modules/prnt/harupdfprint/ \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe

DYNAMIC_MODULE = mod_print_testpdf.so

CPP_OBJS = \
	pdfPrinterFiles.o \
	pdfPrinterDocumentImpl.o \
	mod_print_testpdf.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

