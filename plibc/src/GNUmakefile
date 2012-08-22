TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = 

INCLUDE_LDFLAGS =

INCLUDE_LIBS =

LOCAL_STATIC_LIB_OBJS = \
	strptime.o

LOCAL_STATIC_LIB = \
	plibc.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

ALL_CFLAGS = -O2 -Wall -I. -I../include -Iinclude $(INCLUDE_DIRS) $(INCLUDE_CFLAGS)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)
	@rm -f *.sho++

local_distclean:

local_install:

local_test:
