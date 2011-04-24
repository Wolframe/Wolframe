TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/src/libwolframe.a \
	-ldl

# lua

ifeq ($(WITH_LUA),1)

INCLUDE_CPPFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS += \
	-I$(LUA_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(LUA_LIB_DIR)

INCLUDE_LIBS += \
	$(LUA_LIBS)

endif

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	implementation.o \
	dispatcher.o \
	mtprocHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	mtprocd$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

run: $(CPP_BINS)
	@echo "Running the Wolframe mtproc example..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./mtprocd -c mtproc.conf -f
