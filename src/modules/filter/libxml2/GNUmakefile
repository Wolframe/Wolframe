TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR) \
	$(LIBXML2_INCLUDE_DIRS) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	$(LIBXML2_LIB_DIRS) \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(LIBXML2_LIBS)

DYNAMIC_MODULE = mod_filter_libxml2.so

CPP_OBJS = \
	documentReader.o \
	documentWriter.o \
	inputfilterImpl.o \
	outputfilterImpl.o \
	xsltMapper.o \
	xsltProgramType.o \
	libxml2_filter.o \
	mod_filter_libxml2.o

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

