TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) -I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS =
INCLUDE_LIBS =

DYNAMIC_MODULE = mod_employee_assignment_convert.so

CPP_OBJS = \
	$(TOPDIR)/src/serialize_struct_filtermapBase.o \
	$(TOPDIR)/src/serialize_struct_filtermapParseStack.o \
	$(TOPDIR)/src/serialize_struct_filtermapParse.o \
	$(TOPDIR)/src/serialize_struct_filtermapSerializeStack.o \
	$(TOPDIR)/src/serialize_struct_filtermapSerialize.o \
	employee_assignment_convert.o \
	mod_employee_assignment_convert.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

