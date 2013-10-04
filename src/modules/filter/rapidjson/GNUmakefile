TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) \
	$(RAPIDJSON_INCLUDE_DIRS) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS += \
	-DWITH_RAPIDJSON

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(RAPIDJSON_LIBS)

DYNAMIC_MODULE = mod_filter_rapidjson.so

CPP_OBJS = \
	inputfilterImpl.o \
	outputfilterImpl.o \
	rapidjson_filter.o \
	mod_filter_rapidjson.o

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

