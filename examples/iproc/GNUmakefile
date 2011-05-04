TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/gtest/gtest-all.o \
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

TEST_BINS += \
	tests/iprocHandlerTest$(EXE)

tests/iprocHandlerTest$(EXE): tests/iprocHandlerTest.o $(IPROC_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) -L$(LUA_LIB_DIR) $(LUA_LIBS) $(IPROC_GTEST_OBJS) $(OBJS) $< $(LIBS)


local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test:

# DISABLED UNTIL THEY WORK:
# local_test: $(TEST_BINS)
#	cd tests && ./iprocHandlerTest$(EXE)

run: $(CPP_BINS)
	@echo "Running the Wolframe iproc example..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./iprocd -c iproc.conf -f


