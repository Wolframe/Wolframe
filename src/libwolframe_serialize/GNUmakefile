TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem

STATIC_LIB = libwolframe_serialize.a

DYNAMIC_LIB = libwolframe_serialize.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	$(TOPDIR)/src/serialize_ddl_filtermapParse.o \
	$(TOPDIR)/src/serialize_ddl_filtermapSerialize.o \
	$(TOPDIR)/src/serialize_mapContext.o \
	$(TOPDIR)/src/serialize_struct_filtermapBase.o \
	$(TOPDIR)/src/serialize_struct_filtermapParse.o \
	$(TOPDIR)/src/serialize_struct_filtermapParseStack.o \
	$(TOPDIR)/src/serialize_struct_filtermapSerialize.o \
	$(TOPDIR)/src/serialize_struct_filtermapSerializeStack.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

