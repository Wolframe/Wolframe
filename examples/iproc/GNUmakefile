TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/src/modules/protocol/wolframe \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/src/libwolframed.a \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lwolftest \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/wtest"

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_PYTHON),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(PYTHON_LIB_DIR)"
endif

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_DIRS += \
	$(OPENSSL_INCLUDE_DIRS)
	
INCLUDE_LDFLAGS += \
	$(OPENSSL_LIB_DIRS)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# postgres
ifeq ($(WITH_PGSQL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PGSQL
INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(PGSQL_LIB_DIRS)
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	iprocHandler.o \
	iprocHandlerConfig.o \
	iprocProtocolFiles.o \
	$(TOPDIR)/src/posixMain.o

TEST_BINS = \
	tests/iprocHandlerTest$(EXE) \
	tests/iprocHandlerXMLTest$(EXE)

CPP_BINS = \
	iprocd$(EXE)

iprocd$(EXE): $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

tests/iprocHandlerTest$(EXE): tests/iprocHandlerTest.o $(OBJS) $(TOPDIR)/src/libwolframed.a
tests/iprocHandlerXMLTest$(EXE): tests/iprocHandlerXMLTest.o $(OBJS) $(TOPDIR)/src/libwolframed.a

local_all: $(CPP_BINS)

local_clean:
	-rm -rf tests/result
	-rm -rf tests/temp

local_distclean:

local_install:

local_test: $(TEST_BINS)
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_LUA),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) tests/iprocHandlerTest$(EXE)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) tests/iprocHandlerXMLTest$(EXE)
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe iproc example..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./iprocd -c iproc.conf -f

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
