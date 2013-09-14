TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_langbind \
	$(LIBS_DL)


DYNAMIC_MODULE = mod_command_directmap.so

CPP_OBJS = \
	directmapProgram.o \
	directmapCommandHandler.o \
	directmapCommandHandlerBuilder.o \
	mod_command_directmap.o

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

