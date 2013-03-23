TOPDIR = ../..

SUBDIRS = plugins tests

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	$(QT_CXXFLAGS) -DBUILD_AS_PLUGIN

INCLUDE_DIRS = \
	-I. \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtNetwork \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtUiTools \
	-I$(QT_INCLUDE_DIR)/QtDesigner

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS) \
	-Lplugins
	
INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtUiTools -lQtNetwork -lQtXml \
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
	WolframeClient.o \
	moc_WolframeClient.o \
	FormWidget.o \
	moc_FormWidget.o \
	qcommandline.o \
	moc_qcommandline.o \
	PreferencesDialog.o \
	moc_PreferencesDialog.o \
	FormChooseDialog.o \
	moc_FormChooseDialog.o \
	qrc_qtclient.o \
	connection.o \
	manageServersDialog.o \
	moc_manageServersDialog.o \
	serverDefinitionDialog.o \
	moc_serverDefinitionDialog.o \
	settings.o \
	loginDialog.o \
	moc_loginDialog.o \
	LoadMode.o \
	DebugTerminal.o \
	moc_DebugTerminal.o \
	HistoryLineEdit.o \
	moc_HistoryLineEdit.o
		
CPP_BINS = \
	qtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_MainWindow.cpp: MainWindow.hpp
	$(QT_MOC) $(MOC_FLAGS) MainWindow.hpp -o moc_MainWindow.cpp

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

moc_qcommandline.cpp: qcommandline.h
	$(QT_MOC) $(MOC_FLAGS) qcommandline.h -o moc_qcommandline.cpp

moc_PreferencesDialog.cpp: PreferencesDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) PreferencesDialog.hpp -o moc_PreferencesDialog.cpp

moc_FileChooser.cpp: FileChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) FileChooser.hpp -o moc_FileChooser.cpp

moc_PictureChooser.cpp: PictureChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) PictureChooser.hpp -o moc_PictureChooser.cpp

moc_FormChooseDialog.cpp: FormChooseDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) FormChooseDialog.hpp -o moc_FormChooseDialog.cpp

moc_manageServersDialog.cpp: manageServersDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) manageServersDialog.hpp -o moc_manageServersDialog.cpp

moc_serverDefinitionDialog.cpp: serverDefinitionDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) serverDefinitionDialog.hpp -o moc_serverDefinitionDialog.cpp

moc_loginDialog.cpp: loginDialog.hpp
	$(QT_MOC) $(MOC_FLAGS) loginDialog.hpp -o moc_loginDialog.cpp

moc_HistoryLineEdit.cpp: HistoryLineEdit.hpp
	$(QT_MOC) $(MOC_FLAGS) HistoryLineEdit.hpp -o moc_HistoryLineEdit.cpp

moc_DebugTerminal.cpp: DebugTerminal.hpp
	$(QT_MOC) $(MOC_FLAGS) DebugTerminal.hpp -o moc_DebugTerminal.cpp

ui_MainWindow.h: MainWindow.ui
	$(QT_UIC) -o ui_MainWindow.h MainWindow.ui

ui_manageServersDialog.h: manageServersDialog.ui
	$(QT_UIC) -o ui_manageServersDialog.h manageServersDialog.ui

ifeq ($(WITH_SSL),1)
ui_serverDefinitionDialog.h: serverDefinitionDialog.ui
	$(QT_UIC) -o ui_serverDefinitionDialog.h serverDefinitionDialog.ui
else
ui_serverDefinitionDialogNoSSL.h: serverDefinitionDialogNoSSL.ui
	$(QT_UIC) -o ui_serverDefinitionDialogNoSSL.h serverDefinitionDialogNoSSL.ui
endif

ui_loginDialog.h: loginDialog.ui
	$(QT_UIC) -o ui_loginDialog.h loginDialog.ui

ui_PreferencesDialog.h: PreferencesDialog.ui
	$(QT_UIC) -o ui_PreferencesDialog.h PreferencesDialog.ui

ui_PreferencesDialogDeveloper.h: PreferencesDialogDeveloper.ui
	$(QT_UIC) -o ui_PreferencesDialogDeveloper.h PreferencesDialogDeveloper.ui
	
ui_PreferencesDialogInterface.h: PreferencesDialogInterface.ui
	$(QT_UIC) -o ui_PreferencesDialogInterface.h PreferencesDialogInterface.ui
	
UIS = \
	ui_MainWindow.h \
	ui_manageServersDialog.h \
	ui_loginDialog.h \
	ui_PreferencesDialog.h \
	ui_PreferencesDialogDeveloper.h \
	ui_PreferencesDialogInterface.h

ifeq ($(WITH_SSL),1)
UIS += \
	ui_serverDefinitionDialog.h
else
UIS += \
	ui_serverDefinitionDialogNoSSL.h
endif
	
qtclient.d: ui_MainWindow.h
MainWindow.d: ui_MainWindow.h
manageServersDialog.d: ui_manageServersDialog.h
ifeq ($(WITH_SSL),1)
serverDefinitionDialog.d: ui_serverDefinitionDialog.h
else
serverDefinitionDialog.d: ui_serverDefinitionDialogNoSSL.h
endif
loginDialog.d: ui_loginDialog.h
moc_PreferencesDialog.d: ui_PreferencesDialog.h ui_PreferencesDialogDeveloper.h ui_PreferencesDialogInterface.h
PreferencesDialog.d: ui_PreferencesDialog.h ui_PreferencesDialogDeveloper.h ui_PreferencesDialogInterface.h

qtclient$(EXE): $(CPP_OBJS) qrc_qtclient.o qtclient.o

%.qm : %.ts
	$(QT_LRELEASE) $< -qm $@

%.rcc : %.qrc
	$(QT_RCC) -binary $< -o $@
	
qrc_qtclient.cpp: qtclient.qrc
	$(QT_RCC) -name qtclient $< -o $@

I18NS = \
	i18n/qtclient.de_CH.qm \
	i18n/qtclient.ro_RO.qm

I18NS_TS = $(I18NS:.qm=.ts)

local_all: $(I18NS)

local_clean:
	@-rm -f moc_*.cpp
	@-rm -f $(I18NS)
	@-rm -f $(RESOURCES)
	@-rm -f geany.c++.tags
	@-rm -f $(UIS)
	@-rm -f qrc_qtclient.cpp

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(DESTDIR)$(bindir)

local_uninstall:
	-rm -f $(DESTDIR)$(bindir)/qtclient$(EXE)
	-rmdir $(DESTDIR)$(bindir)

local_test:

run: local_all $(CPP_BINS)
	@echo "Testing Qt client.."
	@LD_LIBRARY_PATH=$(QT_LIB_DIR):plugins ./qtclient -c test.conf

.PHONY: buildi18n
buildi18n:
	$(QT_LUPDATE) -extensions hpp,cpp,ui . -ts i18n/qtclient.de_CH.ts
	$(QT_LUPDATE) -extensions hpp,cpp,ui . -ts i18n/qtclient.ro_RO.ts

geany-ctags:
	CFLAGS="$(ALL_CXXFLAGS) -I$(QT_INCLUDE_DIR)/QtDesigner -Iplugins/filechooser -Iplugins/picturechooser -Iplugins/formwidget" \
		geany -g geany.c++.tags `find . -name '*.[ch]pp'`
