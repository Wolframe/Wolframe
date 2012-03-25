TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-I.

# libharu

ifneq ($(WITH_LOCAL_LIBHARY),1)

INCLUDE_DIRS += \
	$(LIBHPDF_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

ifeq ($(WITH_LOCAL_LIBHARY),1)

INCLUDE_CXXFLAGS += \
	-DWITH_LOCAL_LIBHPDF
	
INCLUDE_DIRS += \
	$(TOPDIR)/libhpdf/include

INCLUDE_LDFLAGS += \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHPDF_LIBS)

endif

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
