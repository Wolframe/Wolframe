TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_langbind

DYNAMIC_MODULE = mod_protocol_wolframe.so

CPP_OBJS = \
	ioblocks.o \
	lineProtocolHandler.o \
	escDlfProtocolHandler.o \
	authProtocolHandler.o \
	baseCryptoProtocolHandler.o \
	doctypeFilterProtocolHandler.o \
	interfaceProtocolHandler.o \
	passwordChangeProtocolHandler.o \
	mainProtocolHandler.o

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
