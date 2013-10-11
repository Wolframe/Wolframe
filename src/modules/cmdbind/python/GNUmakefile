TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS += \
	-DWITH_PYTHON \
	$(PYTHON_CFLAGS)

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind \
	$(PYTHON_LDFLAGS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_langbind \
	$(LIBS_DL) \
	$(PYTHON_LIBS)

DYNAMIC_MODULE = mod_command_python.so

CPP_OBJS = \
	pythonException.o \
	pythonObject.o \
	pythonObjectIterator.o \
	pythonInterpreter.o \
	pythonInterpreterInstance.o \
	pythonStructure.o \
	pythonStructureBuilder.o \
	pythonFunctionProgramType.o \
	mod_command_python.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

