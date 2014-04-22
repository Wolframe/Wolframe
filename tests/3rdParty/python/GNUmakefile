TOPDIR = ../../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-I.

INCLUDE_DIRS += \
	$(PYTHON_CFLAGS)

INCLUDE_LDFLAGS += \
	$(PYTHON_LDFLAGS)

TEST_CPP_BINS = \
	embed$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -rf *.RES

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(PYTHON_LIB_DIR) ./embed my > test.RES
	@diff test.RES test.MUST
	@echo

local_longtest:
