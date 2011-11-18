TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_filesystem

DYNAMIC_MODULE = mod_auth_textfile.so

CPP_OBJS = \
	TextFileAuthConfig.o \
	TextFileAuth.o \
	TextFileAuthModule.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/wolframe/modules/authentication
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(libdir)/wolframe/modules/authentication

local_uninstall:
	-rm "$(libdir)/wolframe/modules/authentication/$(DYNAMIC_MODULE)
	-rmdir $(libdir)/wolframe/modules/authentication
	-rmdir $(libdir)/wolframe/modules

local_test:

