TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	$(LIBXML2_INCLUDE_DIRS) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwfsingleton \
	$(LIBXML2_LIB_DIRS) \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(LIBXML2_LIBS) \
	-lwfsingleton \
	-lwolframe

# libxslt

ifeq ($(WITH_LIBXSLT),1)
INCLUDE_CXXFLAGS += -DWITH_LIBXSLT
INCLUDE_DIRS += $(LIBXSLT_INCLUDE_DIRS)
INCLUDE_LDFLAGS += $(LIBXSLT_LIB_DIRS)
INCLUDE_LIBS += $(LIBXSLT_LIBS)
endif

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

