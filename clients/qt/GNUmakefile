TOPDIR = ../..

SUBDIRS = plugins

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtNetwork \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtUiTools \
	-I$(QT_INCLUDE_DIR)/QtDesigner \
	-I$(QT_INCLUDE_DIR)/QtSql

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS) \
	-Lplugins

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtUiTools -lQtNetwork -lQtXml -lQtSql \
	-lwolframewidgets

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
	moc_MainWindow.o \
	MainWindow.o \
	FormLoader.o \
	moc_FormLoader.o \
	FileFormLoader.o \
	NetworkFormLoader.o \
	DataLoader.o \
	moc_DataLoader.o \
	FileDataLoader.o \
	NetworkDataLoader.o \
	DataHandler.o \
	moc_DataHandler.o \
	DebugTerminal.o \
	moc_DebugTerminal.o \
	HistoryLineEdit.o \
	moc_HistoryLineEdit.o \
	WolframeClient.o \
	moc_WolframeClient.o \
	FormWidget.o \
	moc_FormWidget.o \
	qcommandline.o \
	moc_qcommandline.o \
	LoginDialog.o \
	moc_LoginDialog.o \
	SqliteFormLoader.o \
	SqliteDataLoader.o \
	Preferences.o \
	moc_Preferences.o \
	PreferencesDialog.o \
	moc_PreferencesDialog.o \
	StorageWidget.o \
	moc_StorageWidget.o \
	ManageStorageDialog.o \
	moc_ManageStorageDialog.o \
	FormChooseDialog.o \
	moc_FormChooseDialog.o \
	moc_DefaultMainWindow.o

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

moc_DataHandler.cpp: DataHandler.hpp
	$(QT_MOC) $(MOC_FLAGS) DataHandler.hpp -o moc_DataHandler.cpp

moc_WolframeClient.cpp: WolframeClient.hpp
	$(QT_MOC) $(MOC_FLAGS) WolframeClient.hpp -o moc_WolframeClient.cpp

moc_FormWidget.cpp: FormWidget.hpp
	$(QT_MOC) $(MOC_FLAGS) FormWidget.hpp -o moc_FormWidget.cpp

moc_LoginDialog.cpp: LoginDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) LoginDialog.hpp -o moc_LoginDialog.cpp

moc_qcommandline.cpp: qcommandline.h
	$(QT_MOC) $(MOC_FLAGS) qcommandline.h -o moc_qcommandline.cpp

moc_Preferences.cpp: Preferences.hpp
	$(QT_MOC) $(MOC_FLAGS) Preferences.hpp -o moc_Preferences.cpp

moc_PreferencesDialog.cpp: PreferencesDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) PreferencesDialog.hpp -o moc_PreferencesDialog.cpp

moc_FileChooser.cpp: FileChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) FileChooser.hpp -o moc_FileChooser.cpp

moc_PictureChooser.cpp: PictureChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) PictureChooser.hpp -o moc_PictureChooser.cpp

moc_StorageWidget.cpp: StorageWidget.hpp
	$(QT_MOC) $(MOC_FLAGS) StorageWidget.hpp -o moc_StorageWidget.cpp

moc_ManageStorageDialog.cpp: ManageStorageDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) ManageStorageDialog.hpp -o moc_ManageStorageDialog.cpp
	
moc_FormChooseDialog.cpp: FormChooseDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) FormChooseDialog.hpp -o moc_FormChooseDialog.cpp

moc_DefaultMainWindow.cpp: DefaultMainWindow.hpp
	$(QT_MOC) $(MOC_FLAGS) DefaultMainWindow.hpp -o moc_DefaultMainWindow.cpp

MainWindowUi.hpp: themes/windows/MainWindow.ui
	$(QT_UIC) -o MainWindowUi.hpp themes/windows/MainWindow.ui

DefaultMainWindow.hpp: MainWindowUi.hpp

qtclient$(EXE): $(CPP_OBJS) qtclient.o

FORMS = \
	apps/test/forms/form1.ui \
	apps/test/forms/form2.ui \
	apps/test/forms/form3.ui \
	apps/test/forms/form4.ui

DATA = \
	apps/test/data/form1.xml \
	apps/test/data/domain_form1_vegetable.xml \
	apps/test/data/form2.xml \
	apps/test/data/form3.xml \
	apps/test/data/form4.xml \
	apps/test/data/domain_form4_vegetable.xml
	
THEMES = \
	themes/phone/MainWindow.ui \
	themes/windows/MainWindow.ui

%.qm : %.ts
	$(QT_LRELEASE) $< -qm $@

%.rcc : %.qrc
	$(QT_RCC) -binary $< -o $@

I18NS = \
	i18n/qtclient.de_CH.qm \
	i18n/qtclient.ro_RO.qm \
	themes/windows/MainWindow.de_CH.qm \
	themes/windows/MainWindow.ro_RO.qm \
	themes/phone/MainWindow.de_CH.qm \
	themes/phone/MainWindow.ro_RO.qm \
	apps/test/i18n/init.de_CH.qm \
	apps/test/i18n/init.ro_RO.qm \
	apps/test/i18n/form1.de_CH.qm \
	apps/test/i18n/form1.ro_RO.qm \
	apps/test/i18n/form2.de_CH.qm \
	apps/test/i18n/form2.ro_RO.qm \
	apps/test/i18n/form3.de_CH.qm \
	apps/test/i18n/form3.ro_RO.qm \
	apps/test/i18n/form4.de_CH.qm \
	apps/test/i18n/form4.ro_RO.qm

RESOURCES = \
	apps/configurator/resources/configurator.rcc

I18NS_TS = $(I18NS:.qm=.ts)

local_all: $(I18NS) $(RESOURCES)

local_clean:
	@-rm -f moc_*.cpp
	@-rm -f $(I18NS)
	@-rm -f $(RESOURCES)
	@-rm -f geany.c++.tags
	@-rm -f Makefile
	@-rm -f MainWindowUi.hpp

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(DESTDIR)$(bindir)

local_uninstall:
	-rm -f $(DESTDIR)$(bindir)/qtclient$(EXE)
	-rmdir $(DESTDIR)$(bindir)

local_test:
	@echo "Testing validity of themes.."
	@for i in $(THEMES); do xmllint --noout --schema schemas/qt-ui-4.7.xsd $$i; done
	@echo "Testing validity of forms.."
	@for i in $(FORMS); do xmllint --noout --schema schemas/qt-ui-4.7.xsd $$i; done
	@echo "Testing validity of XML data.."
	@echo "Testing validity of form translation sources.."
	@for i in $(I18NS_TS); do xmllint --noout -dtdvalid schemas/qt-ts-2.0.dtd $$i && echo "$$i validates"; done
	@for i in $(DATA); do xmllint --noout $$i && echo "$$i validates"; done

run: local_all $(CPP_BINS)
	@echo "Testing Qt client.."
	@LD_LIBRARY_PATH=$(QT_LIB_DIR):plugins ./qtclient

.PHONY: buildi18n
buildi18n:
	$(QT_LUPDATE) -extensions hpp,cpp . -ts i18n/qtclient.de_CH.ts
	$(QT_LUPDATE) themes/windows/MainWindow.ui -ts themes/windows/MainWindow.de_CH.ts
	$(QT_LUPDATE) themes/windows/MainWindow.ui -ts themes/windows/MainWindow.ro_RO.ts
	$(QT_LUPDATE) themes/phone/MainWindow.ui -ts themes/phone/MainWindow.de_CH.ts
	$(QT_LUPDATE) themes/phone/MainWindow.ui -ts themes/phone/MainWindow.ro_RO.ts
	$(QT_LUPDATE) apps/test/forms/init.ui -ts apps/test/i18n/init.de_CH.ts
	$(QT_LUPDATE) apps/test/forms/init.ui -ts apps/test/i18n/init.ro_RO.ts
	$(QT_LUPDATE) apps/test/forms/form1.ui -ts apps/test/i18n/form1.de_CH.ts
	$(QT_LUPDATE) apps/test/forms/form1.ui -ts apps/test/i18n/form1.ro_RO.ts
	$(QT_LUPDATE) apps/test/forms/form2.ui -ts apps/test/i18n/form2.de_CH.ts
	$(QT_LUPDATE) apps/test/forms/form2.ui -ts apps/test/i18n/form2.ro_RO.ts
	$(QT_LUPDATE) apps/test/forms/form3.ui -ts apps/test/i18n/form3.de_CH.ts
	$(QT_LUPDATE) apps/test/forms/form3.ui -ts apps/test/i18n/form3.ro_RO.ts
	$(QT_LUPDATE) apps/test/forms/form4.ui -ts apps/test/i18n/form4.de_CH.ts
	$(QT_LUPDATE) apps/test/forms/form4.ui -ts apps/test/i18n/form4.ro_RO.ts

geany-ctags:
	CFLAGS="$(ALL_CXXFLAGS) -I$(QT_INCLUDE_DIR)/QtDesigner -Iplugins/filechooser -Iplugins/picturechooser -Iplugins/formwidget" \
		geany -g geany.c++.tags `find . -name '*.[ch]pp'`
