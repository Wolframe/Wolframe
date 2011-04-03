TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(LUA_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(LUA_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/src/libwolframe.a \
	$(LUA_LIBS) -ldl

# openssl

ifdef WITH_SSL

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	implementation.o \
	implementation_c.o \
	dispatcher.o \
	generators/propertyTree.o \
	luaconfig.o \
	mtprocHandler.o \
	$(TOPDIR)/src/posixMain.o

ifdef WITH_LUA
OBJS += \
	luatypes.o
endif


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
