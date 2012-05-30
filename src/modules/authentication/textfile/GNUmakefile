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

PASSWD_OBJS = \
	passwdFile.o

CPP_OBJS = \
	TextFileAuthConfig.o \
	TextFileAuth.o \
	TextFileAuthModule.o \
	$(PASSWD_OBJS)

PASSWD_LIBS = -lboost_program_options -lboost_system

CPP_BINS = \
	wolfpasswd$(EXE)

wolfpasswd$(EXE): wolfpasswd.o
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(PASSWD_OBJS) $< $(PASSWD_LIBS)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(PASSWD_OBJS)

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

