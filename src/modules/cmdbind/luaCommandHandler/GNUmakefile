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


DYNAMIC_MODULE = mod_lua_command_handler.so

CPP_OBJS = \
	$(TOPDIR)/src/filter_luafilter.o \
	$(TOPDIR)/src/langbind_luaObjects.o \
	$(TOPDIR)/src/langbind_luaGetFunctionClosure.o \
	$(TOPDIR)/src/langbind_luaDebug.o \
	$(TOPDIR)/src/langbind_luaException.o \
	$(TOPDIR)/src/langbind_luaScriptContext.o \
	$(TOPDIR)/src/cmdbind_luaCommandHandler.o \
	mod_lua_command_handler.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:

