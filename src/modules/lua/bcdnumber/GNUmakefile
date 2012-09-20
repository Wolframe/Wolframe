TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR)/lua/src \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)


DYNAMIC_MODULE = mod_lua_bcdnumber.so

CPP_OBJS = \
	$(TOPDIR)/src/langbind_luaException.o \
	$(TOPDIR)/src/langbind_luaBcdNumber.o \
	$(TOPDIR)/src/types_allocators.o \
	$(TOPDIR)/src/types_bcdArithmetic.o \
	mod_lua_bcdnumber.o

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

