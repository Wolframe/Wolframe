TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	$(ICU_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR) \
	$(ICU_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_locale$(BOOST_LIBRARY_TAG) \
	-lboost_system \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_chrono$(BOOST_LIBRARY_TAG) \
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
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

