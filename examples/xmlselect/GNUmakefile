TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/src/libsmerp.a

# openssl

ifdef WITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS = \
	appConfig.o \
	configFile.o \
	xmlselectHandler.o \
	xmlIterator.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	smerp-xmlselect$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

local_clean:

local_distclean:

local_install:
