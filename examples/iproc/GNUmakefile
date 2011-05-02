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
	iprocHandler.o \
	langbind/appObjects.o \
	$(TOPDIR)/src/posixMain.o

ifeq ($(WITH_LUA),1)
OBJS += \
	langbind/luaLog.o \
	langbind/luaConfig.o \
	langbind/luaAppProcessor.o
endif

TEST_BINS =

CPP_BINS = \
	iprocd$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

ifeq ($(WITH_GTEST),1)
TEST_BINS += \
	tests/iprocHandlerTest$(EXE)

GTEST_CPPFLAGS += \
	-I$(GTEST_INCLUDE_DIR)

GTEST_LDFLAGS += \
	-L$(GTEST_LIB_DIR)

GTEST_LIBS += \
	$(GTEST_LIBS)

tests/iprocHandlerTest$(EXE): tests/iprocHandlerTest.o $(IPROC_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) -L$(GTEST_LIB_DIR) -L$(LUA_LIB_DIR) $(LUA_LIBS) $(IPROC_GTEST_OBJS) $(OBJS) $< $(LIBS) $(GTEST_LIBS)
endif



local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test: $(TEST_BINS)
	cd tests

# DISABLED UNTIL THEY WORK: && ./iprocHandlerTest$(EXE)

run: $(CPP_BINS)
	@echo "Running the Wolframe iproc example..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./iprocd -c iproc.conf -f


