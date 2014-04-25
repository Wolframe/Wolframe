TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/3rdParty/lua/src \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_langbind \
	$(TOPDIR)/3rdParty/lua/src/liblua.a

DYNAMIC_MODULE = mod_command_lua.so

LUA_CPP_OBJS = \
	luafilter.o \
	luaObjects.o \
	luaGetFunctionClosure.o \
	luaDebug.o \
	luaDump.o \
	luaScriptContext.o \
	luaFunctionProgramType.o

CPP_OBJS = \
	$(LUA_CPP_OBJS) \
	luaCommandHandler.o \
	luaCommandHandlerConfig.o \
	mod_command_lua.o

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

