TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(INCLUDE_FLAGS_LT)

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
	TextFileAuthConstructor.o \
	TextFileAuth.o \
	TextFileAuthModule.o \
	$(PASSWD_OBJS)

PASSWD_LDFLAGS = \
	$(LDFLAGS_LT)
	
PASSWD_LIBS = \
	$(TOPDIR)/src/libwfsingleton/libwfsingleton.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	-lboost_program_options \
	-lboost_system -lboost_filesystem

CPP_BINS = \
	wolfpasswd$(EXE)

wolfpasswd$(EXE): wolfpasswd.o $(PASSWD_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(PASSWD_LDFLAGS) $(PASSWD_OBJS) $< $(PASSWD_LIBS) $(LIBS_LT)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfpasswd$(EXE) $(DESTDIR)$(bindir)

local_uninstall:
	@-rm -f $(DESTDIR)$(bindir)/wolfpasswd$(EXE)
	@-rmdir $(DESTDIR)$(bindir)
	@-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	@-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

