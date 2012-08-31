TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \
	$(LUA_PLATFORM_LDFLAGS)

INCLUDE_LIBS = \
	$(LUA_PLATFORM_LIBS)

LOCAL_STATIC_LIB_OBJS = \
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

LOCAL_STATIC_LIB = \
	liblua.a

LOCAL_BINS = \
	lua$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

# overwrite flags so they fit to how Lua people compile the software.
# we get too many spurios errors and problems (for instance in abuild
# checks on openSuSE). This doesn't mean they should not be fixed
# though upstream!
ALL_CFLAGS = $(OPTFLAGS) -Wall $(LUA_PLATFORM_CFLAGS) -I. $(SO_COMPILE_FLAGS)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

lua$(EXE): lua.o liblua.a
	$(LINK) -o $@ $(ALL_LDFLAGS) lua.o liblua.a $(LIBS)

local_all: $(LOCAL_STATIC_LIB) $(LOCAL_BINS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)
	@rm -f *.sho++
	@rm -f lua.o

local_distclean:

local_install:

local_test:
