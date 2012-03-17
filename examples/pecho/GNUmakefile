TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

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
	$(LIBS_DL) \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/gtest/gtest-all.o

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
	pechoConfig.o \
	pechoHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	pechod$(EXE)


TEST_BINS += \
	tests/pechoHandlerTest$(EXE)

pechod$(EXE): $(TOPDIR)/src/loadModules-dummy.o $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

tests/pechoHandlerTest$(EXE): tests/pechoHandlerTest.o $(OBJS) $(TOPDIR)/src/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test:$(TEST_BINS)
ifeq ($(RUN_TESTS),1)
	cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./pechoHandlerTest$(EXE)
ifeq ($(WITH_EXPECT),1)
ifneq "$(PLATFORM)" "OPENBSD"
ifneq "$(PLATFORM)" "NETBSD"
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) tests/testPEcho
endif
endif
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./pechod -c pecho.conf -f
