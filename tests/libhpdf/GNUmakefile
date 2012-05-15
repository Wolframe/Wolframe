TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBHPDF_INCLUDE_DIRS) \
	-I/usr/include/rlib \
	-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  

INCLUDE_LDFLAGS = \
	$(LIBHPDF_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBHPDF_LIBS) \
	-lr

TEST_CPP_BINS = \
	testlibhpdf$(EXE) \
	testrlib$(EXE)

# libxml2

ifeq ($(WITH_LIBXML2),1)

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)

endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -f *.pdf

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LIBHPDF_LIB_DIR):$(LIBPNG_LIB_DIR) ./testlibhpdf
	@./testrlib > testrlib.pdf
	@echo

local_longtest:
