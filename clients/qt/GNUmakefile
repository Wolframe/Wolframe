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
	MainWindow.o \
	AbstractFormLoader.o \
	moc_AbstractFormLoader.o \
	FileFormLoader.o \
	DebugTerminal.o \
	moc_DebugTerminal.o

CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_MainWindow.cpp: MainWindow.hpp
	$(QT_DIR)/bin/moc -I. MainWindow.hpp -o moc_MainWindow.cpp

moc_DebugTerminal.cpp: DebugTerminal.hpp
	$(QT_DIR)/bin/moc -I. DebugTerminal.hpp -o moc_DebugTerminal.cpp

moc_AbstractFormLoader.cpp: AbstractFormLoader.hpp
	$(QT_DIR)/bin/moc -I. AbstractFormLoader.hpp -o moc_AbstractFormLoader.cpp

local_clean:
	@-rm -f moc_*.cpp 

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(CPP_BINS) $(bindir)

local_test:
	@echo "Testing validity of forms.."
	@xmllint --noout --schema qt-ui-4.7.xsd forms/form1.ui
	@xmllint --noout --schema qt-ui-4.7.xsd forms/form2.ui
	@echo "Testing Qt client.."
	@./qtclient
		