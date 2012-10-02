TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR) -I$(TOPDIR)/src \
	-I$(TOPDIR) -I$(TOPDIR)/tests \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_locale$(BOOST_LIBRARY_TAG) \
	$(ICU_LIBS)

DYNAMIC_MODULE = mod_normalize_locale.so

CPP_OBJS = \
	localeNormalize.o \
	mod_normalize_locale.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

