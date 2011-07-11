TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_LDFLAGS =

INCLUDE_DIRS = -I.

INCLUDE_LIBS = -ldl

LOCAL_STATIC_LIB = \

LOCAL_STATIC_LIB_OBJS =

OBJS = \
	$(LOCAL_STATIC_LIB_OBJS) \


CPP_BINS = \
	loadModules$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)
	@rm -f *.sho++

local_distclean:

local_install:

local_test:

run: $(CPP_BINS)
	@echo "Running the loadModules example..."
