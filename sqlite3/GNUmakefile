TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

LOCAL_STATIC_LIB_OBJS = \
	sqlite3.o

LOCAL_STATIC_LIB = \
	libsqlite3.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

# override all flags here, not just CFLAGS, compiler.mk
# is geared towards C++ compatibility, Sqlite is strict C
ALL_CFLAGS = -O3 -I. $(SO_COMPILE_FLAGS)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_distclean:

local_install:

local_test:
