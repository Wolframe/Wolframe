TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \

STATIC_LIB = libwolframe_prnt.a

DYNAMIC_LIB = libwolframe_prnt.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	prnt_pdfPrinter.o \
	prnt_pdfPrinterExpression.o \
	prnt_pdfPrinterMethod.o \
	prnt_pdfPrinterVariable.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
