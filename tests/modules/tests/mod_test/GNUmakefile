TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwfshared

INCLUDE_LIBS = \
	-lwfshared \
	-lwolframe

DYNAMIC_MODULE = \
	mod_test.so

CPP_OBJS = \
	mod_test.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

