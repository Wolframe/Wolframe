TOPDIR = ..

SUBDIRS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_LDFLAGS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG)

INCLUDE_LIBS = \

TEST_OBJS = \
	$(TOPDIR)/src/version.o

TEST_CPP_BINS = \
	singletonTest$(EXE) \
	versionTest$(EXE)

include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@echo
	@echo "Testing singleton template..."
	@./singletonTest
	@echo
	@echo "Testing version..."
	@./versionTest
	@echo
