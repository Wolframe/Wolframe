TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe

DYNAMIC_MODULE = mod_proc_echo.so

CPP_OBJS = \
	echoProcessor.o \
	echoProcessorModule.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/wolframe/modules/processor
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(libdir)/wolframe/modules/processor

local_uninstall:
	-rm "$(libdir)/wolframe/modules/processor/$(DYNAMIC_MODULE)
	-rmdir $(libdir)/wolframe/modules/processor
	-rmdir $(libdir)/wolframe/modules

local_test:

