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
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem

STATIC_LIB = libwolframe_serialize.a

DYNAMIC_LIB = libwolframe_serialize.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	ddl_filtermapParse.o \
	ddl_filtermapSerialize.o \
	mapContext.o \
	serializationErrorException.o \
	struct_filtermapBase.o \
	struct_filtermapParse.o \
	struct_filtermapParseStack.o \
	struct_filtermapSerialize.o \
	struct_filtermapSerializeStack.o \
	structOptionParser.o \
	tostringUtils.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

