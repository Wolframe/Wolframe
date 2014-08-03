TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwfsingleton -lwolframe

STATIC_LIB = libwolframe_langbind.a

DYNAMIC_LIB = libwolframe_langbind.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	filterdef.o \
	input.o \
	output.o \
	null_filter.o \
	typingfilter.o \
	typedfilterScope.o \
	inputfilterScope.o \
	joinfilter.o \
	tostringfilter.o \
	ptreefilter.o \
	execContextInputFilter.o \
	ioFilterCommandHandler.o \


-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

