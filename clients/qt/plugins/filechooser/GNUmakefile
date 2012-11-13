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

../../moc_FileChooser.cpp: ../../FileChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) ../../FileChooser.hpp -o ../../moc_FileChooser.cpp

moc_FileChooserPlugin.cpp: FileChooserPlugin.hpp
	$(QT_MOC) $(MOC_FLAGS) FileChooserPlugin.hpp -o moc_FileChooserPlugin.cpp

local_clean:
	@-rm -f moc_*.cpp
	@-rm -f Makefile

local_distclean:

local_install:

local_uninstall:

local_test:
