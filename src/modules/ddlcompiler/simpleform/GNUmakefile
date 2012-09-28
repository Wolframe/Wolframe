TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) -I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_serialize

DYNAMIC_MODULE = mod_ddlcompiler_simpleform.so

CPP_OBJS = \
	ddl_compiler_simpleFormCompiler.o \
	mod_ddlcompiler_simpleform.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

