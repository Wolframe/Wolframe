TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

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
	$(TOPDIR)/src/libwfshared/libwfshared.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

CPP_OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	echoConfig.o \
	echoHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	echod$(EXE)

echod$(EXE): $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:

local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./testEcho
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./echod -c echo.conf -f
