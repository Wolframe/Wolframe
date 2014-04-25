TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

LOCAL_STATIC_LIB_OBJS = \
	sqlite3.o

LOCAL_STATIC_LIB = \
	libsqlite3.a

LOCAL_BINS = \
	sqlite3$(EXE)
	
-include $(TOPDIR)/makefiles/gmake/sub.mk

# override all flags here, not just CFLAGS, compiler.mk
# is geared towards C++ compatibility, Sqlite is strict C
ALL_CFLAGS = $(OPTFLAGS) -I. $(SO_COMPILE_FLAGS)

sqlite3$(EXE): shell.o $(LOCAL_STATIC_LIB)
	$(LINK) -o $@ $(ALL_LDFLAGS) $? $(LIBS) $(LIBS_DL)
	
$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB) $(LOCAL_BINS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS) $(LOCAL_BINS) *.o

local_distclean:

local_install:

local_test:
