TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_LIBGMP),1)
SUBDIRS += libgmp
endif

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include

INCLUDE_LDFLAGS =

INCLUDE_LIBS = \
        $(TOPDIR)/src/libwolframe.a \
	-lboost_thread$(BOOST_LIBRARY_TAG)

# libgmp

ifeq ($(WITH_LIBGMP),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LIBGMP

INCLUDE_LDFLAGS += \
	$(LIBGMP_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBGMP_LIBS) $(LIBGMP_CXX_LIBS)

endif

TEST_CPP_BINS = \
	testbignum$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testbignum 4 5
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) ./testbignum 424234238490238409238402380482304 52342342342342342342342323
	@echo

local_longtest:
