TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/textwolf/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe

DYNAMIC_MODULE = mod_normalize_string.so

CPP_OBJS = \
	trimNormalizeFunction.o \
	asciiNormalizeFunction.o

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

