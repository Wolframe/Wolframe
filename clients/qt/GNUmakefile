TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(QT_DIR)/include \
	-I$(QT_DIR)/include/QtCore \
	-I$(QT_DIR)/include/QtGui \
	-I$(QT_DIR)/include/QtUiTools

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtUiTools
	
OBJS = \
	moc_MainWindow.o \
	MainWindow.o

CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_MainWindow.cpp: MainWindow.hpp
	$(QT_DIR)/bin/moc -I. MainWindow.hpp -o moc_MainWindow.cpp

local_clean:
	@-rm -f moc_MainWindow.cpp

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(CPP_BINS) $(bindir)
