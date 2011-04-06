TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtNetwork \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtUiTools

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR)

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtUiTools -lQtNetwork -lQtXml

MOC_FLAGS = \
	$(INCLUDE_DIRS)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

MOC_FLAGS += \
	-DWITH_SSL
endif

OBJS = \
	moc_MainWindow.o \
	MainWindow.o \
	AbstractFormLoader.o \
	moc_AbstractFormLoader.o \
	FileFormLoader.o \
	DebugTerminal.o \
	moc_DebugTerminal.o \
	HistoryLineEdit.o \
	moc_HistoryLineEdit.o \
	WolframeClient.o \
	moc_WolframeClient.o

CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_MainWindow.cpp: MainWindow.hpp
	$(QT_DIR)/bin/moc $(MOC_FLAGS) MainWindow.hpp -o moc_MainWindow.cpp

moc_HistoryLineEdit.cpp: HistoryLineEdit.hpp
	$(QT_DIR)/bin/moc $(MOC_FLAGS) HistoryLineEdit.hpp -o moc_HistoryLineEdit.cpp

moc_DebugTerminal.cpp: DebugTerminal.hpp
	$(QT_DIR)/bin/moc $(MOC_FLAGS) DebugTerminal.hpp -o moc_DebugTerminal.cpp

moc_AbstractFormLoader.cpp: AbstractFormLoader.hpp
	$(QT_DIR)/bin/moc $(MOC_FLAGS) AbstractFormLoader.hpp -o moc_AbstractFormLoader.cpp

moc_WolframeClient.cpp: WolframeClient.hpp
	$(QT_DIR)/bin/moc $(MOC_FLAGS) WolframeClient.hpp -o moc_WolframeClient.cpp

local_clean:
	@-rm -f moc_*.cpp

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(bindir)

FORMS = \
	forms/form1.ui \
	forms/form2.ui

THEMES = \
	themes/phone/MainWindow.ui \
	themes/windows/MainWindow.ui

local_test:
	@echo "Testing validity of themes.."
	@for i in $(THEMES); do xmllint --noout --schema qt-ui-4.7.xsd $$i; done
	@echo "Testing validity of forms.."
	@for i in $(FORMS); do xmllint --noout --schema qt-ui-4.7.xsd $$i; done

run: $(CPP_BINS)
	@echo "Testing Qt client.."
	@./qtclient
