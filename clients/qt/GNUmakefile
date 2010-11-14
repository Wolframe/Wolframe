TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(QT_DIR)/include/QtCore \
	-I$(QT_DIR)/include/QtGui

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib

INCLUDE_LIBS = \
	-lQtCore -lQtGui
	
OBJS = \
	MainWindow.o

CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(CPP_BINS) $(bindir)
