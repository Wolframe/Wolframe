TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_serialize \
	-lboost_filesystem

DYNAMIC_MODULE = mod_auth_test.so

CPP_OBJS = \
	testAuthentication.o \
	mod_auth_test.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	@-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	@-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

