TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR) -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	-lboost_filesystem -lboost_system

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

# libhpdf
ifeq ($(WITH_LIBHPDF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF
endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2
endif

# libxslt
ifeq ($(WITH_LIBXSLT),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXSLT
endif

# icu
ifeq ($(WITH_ICU),1)
INCLUDE_CXXFLAGS += \
	-DWITH_ICU
endif

MY_STATIC_LIB = \
	wolftest.a

MY_STATIC_LIB_OBJS = \
	testDescription.o \
	testModules.o

CPP_OBJS = \
	testDescription.o \
	testModules.o

CPP_BINS = \
	wtestcheck$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(MY_STATIC_LIB)

$(MY_STATIC_LIB): $(MY_STATIC_LIB_OBJS)
	ar rcs $(MY_STATIC_LIB) $(MY_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(MY_STATIC_LIB)

local_distclean:

local_install:

local_uninstall:

local_test:
