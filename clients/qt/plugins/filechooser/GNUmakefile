TOPDIR = ../../../..

SUBDIRS =

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-DQT_PLUGIN -DQT_SHARED -DQT_NO_DEBUG \
	-DQDESIGNER_EXPORT_WIDGETS

INCLUDE_DIRS = \
	-I. \
	-I../.. \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtDesigner

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS)

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtDesigner

MOC_FLAGS = \
	$(INCLUDE_DIRS)

CPP_OBJS = \
	../../FileChooser.o \
	../../moc_FileChooser.o \
	FileChooserPlugin.o \
	moc_FileChooserPlugin.o

DYNAMIC_MODULE = libwolframefilechooser.so

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
