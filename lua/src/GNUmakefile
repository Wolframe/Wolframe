TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \
	$(LUA_PLATFORM_LDFLAGS)

INCLUDE_LIBS = \
	$(LUA_PLATFORM_LIBS)

STATIC_LIB = liblua.a

DYNAMIC_LIB = liblua.so
DYNAMIC_LIB_MAJOR = 5
DYNAMIC_LIB_MINOR = 2
DYNAMIC_LIB_PATCH = 0

OBJS = \
	lapi.o \
	lauxlib.o \
	lbaselib.o \
	lbitlib.o \
	lcode.o \
	lcorolib.o \
	lctype.o \
	ldblib.o \
	ldebug.o \
	ldo.o \
	ldump.o \
	lfunc.o \
	lgc.o \
	linit.o \
	liolib.o \
	llex.o \
	lmathlib.o \
	lmem.o \
	loadlib.o \
	lobject.o \
	lopcodes.o \
	loslib.o \
	lparser.o \
	lstate.o \
	lstring.o \
	lstrlib.o \
	ltable.o \
	ltablib.o \
	ltm.o \
	lundump.o \
	lvm.o \
	lzio.o

BINS = \
	lua$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

# overwrite flags so they fit to how Lua people compile the software.
# we get too many spurios errors and problems (for instance in abuild
# checks on openSuSE). This doesn't mean they should not be fixed
# though upstream!
ALL_CFLAGS = $(OPTFLAGS) -Wall $(LUA_PLATFORM_CFLAGS) -I. $(SO_COMPILE_FLAGS)

local_all:

local_clean:

local_distclean:

local_install:

local_test:
