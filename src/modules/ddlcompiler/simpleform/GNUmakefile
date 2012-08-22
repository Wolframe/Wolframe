TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) -I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS =
INCLUDE_LIBS =

DYNAMIC_MODULE = mod_ddlcompiler_simpleform.so

CPP_OBJS = \
	$(TOPDIR)/src/ddl_compiler_simpleFormCompiler.o \
	$(TOPDIR)/src/ddl_atomicType.o \
	$(TOPDIR)/src/ddl_structType.o \
	$(TOPDIR)/src/libwolframe/resolvePath.o \
	$(TOPDIR)/src/libwolframe/stringUtils.o \
	mod_ddlcompiler_simpleform.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

