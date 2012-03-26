TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-I.

# libharu

ifneq ($(WITH_LOCAL_LIBHPDF),1)

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

ifeq ($(WITH_LOCAL_LIBHPDF),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LOCAL_LIBHPDF
	
INCLUDE_DIRS += \
	-I$(TOPDIR)/libhpdf/include

INCLUDE_LIBS += \
	$(TOPDIR)/libhpdf/src/libhpdf.a

endif

INCLUDE_LDFLAGS += \
	$(LIBPNG_LIB_DIRS) $(LIBPNG_LIBS)

INCLUDE_LIBS += \
	$(LIBPNG_LIBS) $(LIBZ_LIBS)

TEST_CPP_BINS = \
	testlibhpdf$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -f *.pdf

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LIBHPDF_LIB_DIR) ./testlibhpdf
	@echo

local_longtest:
