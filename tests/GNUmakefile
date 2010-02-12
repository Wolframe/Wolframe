TOPDIR = ..

BOOST_COMPILER = gcc44

# SUBDIRS = openldap ldapcpp pam sasl aaaa
SUBDIRS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I/usr/local/include/boost
#	-I/usr/include/boost

INCLUDE_LDFLAGS = \
	-static -L/usr/lib

INCLUDE_LIBS = \
	$(TOPDIR)/src/protocol.o \
	$(TOPDIR)/src/protocolimpl.o \
	-lboost_unit_test_framework \
	-lboost_thread \
#	-lboost_unit_test_framework


TEST_CPP_BINS = \
	singletonTest$(EXE) \
	singletonSpeedWflag$(EXE) \
	singletonSpeedWOflag$(EXE) \

include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@echo "Testing singleton template..."
	@./singletonTest
	@echo "Testing singleton template speed ..."
	@time ./singletonSpeedWflag
	@time ./singletonSpeedWOflag
