TOPDIR = ..

SUBDIRS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \

INCLUDE_LDFLAGS = \

#	-static -L/usr/lib

INCLUDE_LIBS = -static \
	-lboost_unit_test_framework \
	-lboost_thread


TEST_CPP_BINS = \
	singletonTest$(EXE) \
	version$(EXE) \

include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@echo "Testing singleton template..."
	@./singletonTest
	@echo "Testing version..."
	@./version
