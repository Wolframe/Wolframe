TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtNetwork \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtUiTools

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS)

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtUiTools -lQtNetwork -lQtXml

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

OBJS = \
	moc_MainWindow.o \
	MainWindow.o \
	FormLoader.o \
	moc_FormLoader.o \
	FileFormLoader.o \
	DataLoader.o \
	moc_DataLoader.o \
	FileDataLoader.o \
	DataHandler.o \
	DebugTerminal.o \
	moc_DebugTerminal.o \
	HistoryLineEdit.o \
	moc_HistoryLineEdit.o \
	WolframeClient.o \
	moc_WolframeClient.o \
	FormWidget.o \
	moc_FormWidget.o

CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_MainWindow.cpp: MainWindow.hpp
	$(QT_MOC) $(MOC_FLAGS) MainWindow.hpp -o moc_MainWindow.cpp

moc_HistoryLineEdit.cpp: HistoryLineEdit.hpp
	$(QT_MOC) $(MOC_FLAGS) HistoryLineEdit.hpp -o moc_HistoryLineEdit.cpp

moc_DebugTerminal.cpp: DebugTerminal.hpp
	$(QT_MOC) $(MOC_FLAGS) DebugTerminal.hpp -o moc_DebugTerminal.cpp

moc_FormLoader.cpp: FormLoader.hpp
	$(QT_MOC) $(MOC_FLAGS) FormLoader.hpp -o moc_FormLoader.cpp

moc_DataLoader.cpp: DataLoader.hpp
	$(QT_MOC) $(MOC_FLAGS) DataLoader.hpp -o moc_DataLoader.cpp

moc_WolframeClient.cpp: WolframeClient.hpp
	$(QT_MOC) $(MOC_FLAGS) WolframeClient.hpp -o moc_WolframeClient.cpp

moc_FormWidget.cpp: FormWidget.hpp
	$(QT_MOC) $(MOC_FLAGS) FormWidget.hpp -o moc_FormWidget.cpp

	
FORMS = \
	forms/form1.ui \
	forms/form2.ui

DATA = \
	data/form1.xml

THEMES = \
	themes/phone/MainWindow.ui \
	themes/windows/MainWindow.ui

%.qm : %.ts
	$(QT_LRELEASE) $< -qm $@

I18NS = \
	i18n/form1.de_CH.qm \
	themes/windows/qtclient.de_CH.qm \
	themes/phone/qtclient.de_CH.qm

local_all: $(I18NS)

local_clean:
	@-rm -f moc_*.cpp
	@-rm -f $(I18NS)

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(DESTDIR)$(bindir)

local_uninstall:
	-rm -f $(DESTDIR)$(bindir)/qtclient$(EXE)
	-rmdir $(DESTDIR)$(bindir)

local_test:
	@echo "Testing validity of themes.."
	@for i in $(THEMES); do xmllint --noout --schema qt-ui-4.7.xsd $$i; done
	@echo "Testing validity of forms.."
	@for i in $(FORMS); do xmllint --noout --schema qt-ui-4.7.xsd $$i; done
	@echo "Testing validity of XML data.."
	@for i in $(DATA); do xmllint --noout $$i; done

run: local_all $(CPP_BINS)
	@echo "Testing Qt client.."
	@LD_LIBRARY_PATH=$(QT_LIB_DIR) ./qtclient

.PHONY: buildi18n
buildi18n:
	$(QT_LUPDATE) themes/windows/MainWindow.ui -ts themes/windows/qtclient.de_CH.ts
	$(QT_LUPDATE) lupdate themes/phonr/MainWindow.ui -ts themes/phone/qtclient.de_CH.ts
	$(QT_LUPDATE) lupdate forms/form1.ui -ts i18n/form1.de_CH.ts
