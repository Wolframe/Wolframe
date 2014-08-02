TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/protocol/wolframe \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/wtest \
	-L$(BOOST_LIB_DIR) \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lwolftest \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

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

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	pechoConfig.o \
	pechoHandler.o \
	pechoProtocolFiles.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	pechod$(EXE)


TEST_BINS += \
	tests/pechoHandlerTest$(EXE)

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(LIBXML2_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/wtest"

pechod$(EXE): $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a

tests/pechoHandlerTest$(EXE): tests/pechoHandlerTest.o $(OBJS) $(TOPDIR)/src/libwolframed.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test:$(TEST_BINS)
ifeq ($(RUN_TESTS),1)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) tests/pechoHandlerTest$(EXE)
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) tests/testPEcho
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./pechod -c pecho.conf -f

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
