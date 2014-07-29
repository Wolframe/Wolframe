TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include  \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	$(FREEIMAGE_INCLUDE_DIRS) \
	$(FREEIMAGEPLUS_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe \
	$(FREEIMAGE_LIB_DIRS) \
	$(FREEIMAGEPLUS_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe_serialize \
	$(FREEIMAGEPLUS_LIBS) \
	$(FREEIMAGE_LIBS)

DYNAMIC_MODULE = mod_graphix.so

CPP_OBJS = \
	graphix.o

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

