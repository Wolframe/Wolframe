TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CFLAGS = \
	$(LUA_PLATFORM_CFLAGS)

INCLUDE_DIRS = \
	-I.

INCLUDE_LDFLAGS = \
	$(LUA_PLATFORM_LDFLAGS)

INCLUDE_LIBS =

LOCAL_STATIC_LIB_OBJS = \
	lapi.o \
	lauxlib.o \
	lbaselib.o \
	lcoco.o \
	lcode.o \
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
	lzio.o \
	print.o

LOCAL_STATIC_LIB = \
	liblua.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB)


local_clean:
	@rm -f $(LOCAL_STATIC_LIB)

local_clean:

local_distclean:

local_install:

local_test:
