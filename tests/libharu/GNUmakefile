TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

# libharu

ifneq ($(WITH_LOCAL_LIBHARY),1)

INCLUDE_DIRS += \
	$(LIBHARU_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBHARU_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBHARU_LIBS)

endif

TEST_CPP_BINS = \
	testlibharu$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -f *.pdf

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LIBHARU_LIB_DIR) ./testlibharu
	@echo

local_longtest:
