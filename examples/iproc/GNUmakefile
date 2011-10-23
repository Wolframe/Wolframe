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
	$(LIBS_DL)

# lua

ifeq ($(WITH_LUA),1)
INCLUDE_CPPFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS += \
	-I$(TOPDIR)/lua/src

INCLUDE_LIBS += \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)
endif

# libxml2

ifeq ($(WITH_LIBXML2),1)
INCLUDE_CPPFLAGS += \
	-DWITH_LIBXML2

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)
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
	../../src/loadModules-dummy.o \
	iprocHandler.o \
	langbind/appObjects.o \
	filters/textwolf_filter.o \
	filters/textwolf_filterBase.o \
	filters/char_filter.o \
	filters/line_filter.o \
	serialize/directmapBase.o \
	$(TOPDIR)/src/posixMain.o

#PF: temporarily disabled: 	serialize/directmapCompile.o \

ifeq ($(WITH_LUA),1)
OBJS += \
	serialize/luamapBase.o \
	langbind/luaDebug.o \
	langbind/luaConfig.o \
	langbind/luaAppProcessor.o
endif

ifeq ($(WITH_LIBXML2),1)
OBJS += \
	filters/libxml2_filter.o
endif

TEST_BINS =

CPP_BINS = \
	iprocd$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

TEST_BINS += \
	tests/iprocHandlerTest$(EXE) \
	tests/iprocHandlerXMLTest$(EXE) \
	tests/luamapParser$(EXE) \
	tests/directmapParser$(EXE)

#PF: temporarily disabled: 	tests/directmapCompiler$(EXE) \


tests/iprocHandlerTest$(EXE): tests/iprocHandlerTest.o $(OBJS) $(TOPDIR)/src/libwolframe.a
tests/iprocHandlerXMLTest$(EXE): tests/iprocHandlerXMLTest.o $(OBJS) $(TOPDIR)/src/libwolframe.a
tests/luamapParser$(EXE): tests/luamapParser.o $(OBJS) $(TOPDIR)/src/libwolframe.a
tests/directmapParser$(EXE): tests/directmapParser.o $(OBJS) $(TOPDIR)/src/libwolframe.a
tests/directmapCompiler$(EXE): tests/directmapCompiler.o $(OBJS) $(TOPDIR)/src/libwolframe.a

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test: $(TEST_BINS)
	cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./iprocHandlerTest$(EXE)
	cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./iprocHandlerXMLTest$(EXE)

#PF: temporarily disabled: cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./luamapParser$(EXE)
#PF: temporarily disabled: cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./directmapCompiler$(EXE) data/directmap_compiler.frm
#PF: temporarily disabled: cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./directmapParser$(EXE)

run: $(CPP_BINS)
	@echo "Running the Wolframe iproc example..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./iprocd -c iproc.conf -f

