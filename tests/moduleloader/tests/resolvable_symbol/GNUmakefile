TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwfsingleton \
	-L../libbla

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lbla

DYNAMIC_MODULE = \
	resolvable_symbol.so

CPP_OBJS = \
	resolvable_symbol.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

