TEMPLATE = app

TARGET = qtclient

CONFIG += thread qt debug uitools

# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
dir1.source = apps
DEPLOYMENTFOLDERS = dir1

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

QT += core gui network sql

SOURCES += \
	MainWindow.cpp \
	FormLoader.cpp \
	FileFormLoader.cpp \
	NetworkFormLoader.cpp \
	SqliteFormLoader.cpp \
	DataLoader.cpp \
	FileDataLoader.cpp \
	NetworkDataLoader.cpp \
	SqliteDataLoader.cpp \
	DataHandler.cpp \
	DebugTerminal.cpp \
	HistoryLineEdit.cpp \
	WolframeClient.cpp \
	FormWidget.cpp \
	qcommandline.cpp \
	LoginDialog.cpp \
	qtclient.cpp \
	Preferences.cpp \
	PreferencesDialog.cpp \
	FileChooser.cpp \
	StorageWidget.cpp \
	ManageStorage.cpp

HEADERS += \
	DataHandler.hpp \
	DataLoader.hpp \
	FileDataLoader.hpp \
	FileFormLoader.hpp \
	SqliteFormLoader.hpp \
	SqliteDataLoader.hpp \
	DebugTerminal.hpp \
	FormLoader.hpp \
	FormWidget.hpp \
	HistoryLineEdit.hpp \
	LoginDialog.hpp \
	qcommandline.h \
	WolframeClient.hpp \
	MainWindow.hpp \
	NetworkDataLoader.hpp \
	NetworkFormLoader.hpp \
	Version.hpp \
	Preferences.hpp \
	PreferencesDialog.hpp \
	FileChooser.hpp \
	LoadMode.hpp \
	StorageWidget.hpp \
	ManageStorageDialog.hpp

FORMS +=

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

TRANSLATIONS += \
	apps/test/i18n/form1.de_CH.ts \
	apps/test/i18n/form1.ro_RO.ts \
	apps/test/i18n/form2.de_CH.ts \
	apps/test/i18n/form3.de_CH.ts \
	themes/phone/qtclient.de_CH.ts \
	themes/phone/qtclient.ro_RO.ts \
	themes/windows/qtclient.de_CH.ts \
	themes/windows/qtclient.ro_RO.ts

QT_LRELEASE = $$QMAKE_MOC 
QT_LRELEASE ~= s,moc,lrelease,

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$QT_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

OTHER_FILES += \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/values-nb/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-ro/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values-ms/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-es/strings.xml \
    android/res/layout/splash.xml

