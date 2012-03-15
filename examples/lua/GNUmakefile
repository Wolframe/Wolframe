TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR)/lua/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	-lboost_date_time \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	$(TOPDIR)/src/loadModules-dummy.o \
	luaConfig.o \
	luaHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	luad$(EXE)

luad$(EXE): $(TOPDIR)/src/loadModules-dummy.o $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
ifneq "$(PLATFORM)" "OPENBSD"
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./testLua
endif
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./luad -c lua.conf -f
