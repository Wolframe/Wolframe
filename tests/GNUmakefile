TOPDIR = ..

SUBDIRS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \

INCLUDE_LDFLAGS = \
	-lboost_unit_test_framework \
	-lboost_thread

INCLUDE_LIBS = \

TEST_CPP_BINS = \
	singletonTest$(EXE) \
	version$(EXE)

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
	@./version
	@echo
