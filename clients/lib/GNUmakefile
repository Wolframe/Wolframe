TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system

# openssl
ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)
endif


STATIC_LIB = libwolframe_client.a

DYNAMIC_LIB = libwolframe_client.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

OBJS =

CPP_OBJS = \
	connection.o \
	protocol.o \
	session.o

DIST_HEADERS = \
	connection.hpp \
	exportable.hpp \
	protocol.hpp \
	session.hpp

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/libclient
	$(INSTALL) -m 644 $(DIST_HEADERS) $(DESTDIR)$(includedir)/wolframe/libclient

local_uninstall:
	-rm -rf $(DESTDIR)$(includedir)/wolframe/libclient/*
	-rmdir $(DESTDIR)$(includedir)/wolframe/libclient
	-rmdir $(DESTDIR)$(includedir)/wolframe

local_test:
