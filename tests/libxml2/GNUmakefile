TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

# libxml2

ifeq ($(WITH_LIBXML2),1)

INCLUDE_DIRS = \
	-I. \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBXML2_LIBS)

endif

TEST_CPP_BINS = \
	testlibxml1$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@./testlibxml1 test.xml
	@echo
	@echo

local_longtest:
