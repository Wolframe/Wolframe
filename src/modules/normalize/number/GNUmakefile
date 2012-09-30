TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR) -I$(TOPDIR)/src \
	-I$(TOPDIR) -I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe

DYNAMIC_MODULE = mod_normalize_number.so

CPP_OBJS = \
	trimNormalizeFunction.o \
	integerNormalizeFunction.o \
	floatNormalizeFunction.o \
	numberNormalize.o \
	mod_normalize_number.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

