TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_serialize

INCLUDE_DIRS = \
	-I. -I$(TOPDIR) -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lwolframe -lwolframe_serialize -lwolframe_database \
	-lboost_filesystem

STATIC_LIB = libwolframe_langbind.a

DYNAMIC_LIB = libwolframe_langbind.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	protocol_ioblocks.o \
	appObjects.o \
	null_filter.o \
	typingfilter.o \
	typedfilterScope.o \
	inputfilterScope.o \
	tostringfilter.o \
	ptreefilter.o \
	charsetEncodings.o \
	cmdbind_lineCommandHandler.o \
	cmdbind_execCommandHandler.o \
	cmdbind_doctypeFilterCommandHandler.o \
	cmdbind_contentBufferCommandHandlerEscDLF.o \
	cmdbind_ioFilterCommandHandlerEscDLF.o


-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

