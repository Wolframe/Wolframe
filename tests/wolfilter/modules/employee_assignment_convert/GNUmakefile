TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe_serialize

INCLUDE_LIBS = \
	-lwolframe_serialize \
	$(LIBS_DL)

DYNAMIC_MODULE = mod_employee_assignment_convert.so

CPP_OBJS = \
	employee_assignment_convert.o \
	mod_employee_assignment_convert.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

