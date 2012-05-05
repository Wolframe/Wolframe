TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lboost_filesystem

# openssl
ifeq ($(WITH_SSL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_SSL
endif

# lua
ifeq ($(WITH_LUA),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LUA
endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2
endif

# libgmp
ifeq ($(WITH_LIBGMP),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBGMP
endif

LOCAL_STATIC_LIB = \
	wolftest.a

LOCAL_STATIC_LIB_OBJS = \
	testDescription.o

CPP_OBJS =

CPP_BINS = \
	wtestcheck$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

wtestcheck$(EXE): wtestcheck.o $(OBJS) $(CPP_OBJS) $(LOCAL_STATIC_LIB)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $<  $(LOCAL_STATIC_LIB) $(LIBS)

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)

local_distclean:

local_install:

local_uninstall:

local_test:

