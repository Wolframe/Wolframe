TOPDIR = ../../../..

SUBDIRS =

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	$(QT_CXXFLAGS) -DBUILD_AS_PLUGIN

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/clients/qt \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtNetwork \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtUiTools \
	-I$(QT_INCLUDE_DIR)/QtDesigner

ifeq ($(QT_VERSION),5)
INCLUDE_DIRS += \
	-I$(QT_INCLUDE_DIR)/QtWidgets \
	-I$(QT_INCLUDE_DIR)/QtDesigner 
endif

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS)
	
ifeq ($(QT_VERSION),4)
INCLUDE_LIBS = \
	-lQtCore
endif

ifeq ($(QT_VERSION),5)
INCLUDE_LIBS = \
	-lQt5Core
endif

MOC_FLAGS = \
	$(INCLUDE_DIRS)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

MOC_FLAGS += \
	-DWITH_SSL
	
endif


CPP_OBJS = \
	../../DataTree.o
		
CPP_BINS = \
	testDataTree$(EXE)

all: $(CPP_BINS)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

testDataTree$(EXE): $(CPP_OBJS) testDataTree.o

local_clean:
	@-rm -f $(CPP_OBJS) testDataTree.o

local_distclean:

local_install:

local_uninstall:

local_test:
	@echo "Test DataTree Operations:"
	@./testDataTree

