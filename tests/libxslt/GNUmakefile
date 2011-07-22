TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \
	-I.

# libxml2

ifeq ($(WITH_LIBXML2),1)

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)

endif

# libxslt

ifeq ($(WITH_LIBXSLT),1)

INCLUDE_DIRS += \
	$(LIBXSLT_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXSLT_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXSLT_LIBS)

endif

TEST_CPP_BINS = \
	testlibxslt1$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:
	@-rm -rf *.RES

local_test:
	@./testlibxslt1 test1.xslt test1.xml > test1.RES
	@diff test1.RES test1.MUST
	@echo
	@echo

local_longtest:
