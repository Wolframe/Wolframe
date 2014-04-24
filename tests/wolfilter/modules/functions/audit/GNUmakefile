TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe_serialize
	
DYNAMIC_MODULE = mod_audit.so

CPP_OBJS = \
	mod_audit.o \
	audit.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

