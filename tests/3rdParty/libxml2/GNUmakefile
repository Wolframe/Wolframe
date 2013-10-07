TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
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

TEST_CPP_BINS = \
	testlibxml2$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -f *.RES
	@-rm -f *.DIFF

local_distclean:

ENCODINGS=UTF-8 UCS-2LE UCS-2BE UTF-16LE UTF-16BE

local_test: $(TEST_CPP_BINS)
	@for e in $(ENCODINGS); do \
		echo "Testing libxml2 with encoding $$e"; \
		LD_LIBRARY_PATH=$(LIBXML2_LIB_DIR) ./testlibxml2 test-$$e.xml > $$e.RES; \
		diff $$e.MUST $$e.RES > $$e.DIFF; \
	done

local_longtest:

testdata:
	@for e in $(ENCODINGS); do \
		sed "s/encoding=\"UTF-8\"/encoding=\"$$e\"/" test-orig.xml > x; \
		iconv -f UTF-8 -t $$e -o test-$$e.xml x; \
	done

