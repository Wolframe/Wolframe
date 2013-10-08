TOPDIR = ../../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-I.

# libxml2

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)

# libxslt

INCLUDE_DIRS += \
	$(LIBXSLT_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXSLT_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXSLT_LIBS)

TEST_CPP_BINS = \
	testlibxslt1$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -rf *.RES

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LIBXSLT_LIB_DIR) ./testlibxslt1 test1.xslt test1.xml > test1.RES
	@diff test1.RES test1.MUST
	@echo
	@echo

local_longtest:
