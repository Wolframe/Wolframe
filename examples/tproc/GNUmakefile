TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include \
	-I$(TOPDIR)/wtest

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/wtest/wolftest.a \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwolframe_serialize/libwolframe_serialize.a \
	$(LIBS_DL)

# lua
ifeq ($(WITH_LUA),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS += \
	-I$(TOPDIR)/lua/src

INCLUDE_LIBS += \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)
endif

# libhpdf
ifeq ($(WITH_LIBHPDF),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBHPDF

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)
endif

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
	tprocHandler.o \
	tprocHandlerConfig.o \
	$(TOPDIR)/src/posixMain.o

TEST_BINS = \
	tests/tprocHandlerTest$(EXE)

CPP_BINS = \
	tprocd$(EXE)

tprocd$(EXE): $(TOPDIR)/src/loadModules-dummy.o $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframe.a $(TOPDIR)/src/libwolframe/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:
	rm -Rf tests/temp

local_distclean:

local_install:

local_test: $(TEST_BINS)
ifeq ($(RUN_TESTS),1)
	mkdir -p tests/temp
	cd tests && LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(LIBXML2_LIB_DIR) ./tprocHandlerTest$(EXE)
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe tproc example..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(LIBXML2_LIB_DIR) ./tprocd -c tproc.conf -f

