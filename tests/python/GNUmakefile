TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-I.

INCLUDE_DIRS += \
	$(PYTHON_CFLAGS)

INCLUDE_LDFLAGS += \
	$(PYTHON_LDFLAGS)

INCLUDE_LIBS += \
	$(PYTHON_LIBS)

TEST_CPP_BINS = \
	embed$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -rf *.RES

local_distclean:

local_test: $(TEST_CPP_BINS)
	@./embed my > test.RES
	@diff test.RES test.MUST
	@echo

local_longtest:
