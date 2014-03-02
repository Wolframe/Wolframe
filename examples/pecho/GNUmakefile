TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfshared \
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
	-lwfshared \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lwolftest \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(LIBS_DL)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# postgres
ifeq ($(WITH_PGSQL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PGSQL
INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	pechoConfig.o \
	pechoHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	pechod$(EXE)


TEST_BINS += \
	tests/pechoHandlerTest$(EXE)

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfshared:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(LIBXML2_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/wtest"

pechod$(EXE): $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

tests/pechoHandlerTest$(EXE): tests/pechoHandlerTest.o $(OBJS) $(TOPDIR)/src/libwolframe.a

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
