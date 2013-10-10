TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_langbind \
	$(LIBS_DL)


DYNAMIC_MODULE = mod_command_mylang.so

CPP_OBJS = \
	mylangInterpreterInstance.o \
	mylangInterpreter.o \
	mylangStructure.o \
	mylangStructureBuilder.o \
	mylangFunctionProgramType.o \
	mod_command_mylang.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

