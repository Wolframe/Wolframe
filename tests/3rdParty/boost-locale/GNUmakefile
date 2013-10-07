TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	$(ICU_LIB_DIRS)

INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	-lboost_locale$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(ICU_LIBS)

TEST_OBJS =

TEST_CPP_BINS = \
	boostLocaleTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(ICU_LIB_DIR) LANG=en_US.UTF8 ./boostLocaleTest
	@echo
endif
