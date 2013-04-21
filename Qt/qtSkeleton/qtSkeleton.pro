TEMPLATE = app

TARGET = qtSkeleton

CONFIG += thread qt uitools designer debug

#DEFINES += WITH_SSL

INCLUDEPATH += .

#LIBS += plugins/release/wolframewidgets.lib
#LIBS += plugins/build/Release/libwolframewidgets.dylib

QT += core gui network sql

SOURCES += \
	MainWindow.cpp \
	DataLoader.cpp \
	FileDataLoader.cpp \
	NetworkDataLoader.cpp \
	DataHandler.cpp \
	WolframeClient.cpp \
	PreferencesDialog.cpp \
	connection.cpp \
	manageServersDialog.cpp \
	serverDefinitionDialog.cpp \
	settings.cpp \
	loginDialog.cpp \
	LoadMode.cpp \
	DebugTerminal.cpp \
	HistoryLineEdit.cpp \
	qtSkeleton.cpp

HEADERS += \
	MainWindow.hpp \
	DataLoader.hpp \
	FileDataLoader.hpp \
	NetworkDataLoader.hpp \
	DataHandler.hpp \
	WolframeClient.hpp \
	PreferencesDialog.hpp \
	connection.hpp \
	manageServersDialog.hpp \
	serverDefinitionDialog.hpp \
	settings.hpp \
	loginDialog.hpp \
	LoadMode.hpp \
	DebugTerminal.hpp \
	HistoryLineEdit.hpp

RESOURCES = \
	qtSkeleton.qrc

FORMS += \
	MainWindow.ui \
	PreferencesDialog.ui \
	PreferencesDialogDeveloper.ui \
	PreferencesDialogInterface.ui \
	loginDialog.ui \
	manageServersDialog.ui \
	serverDefinitionDialog.ui

TRANSLATIONS += \
	i18n/qtSkeleton.de_CH.ts \
	i18n/qtSkeleton.ro_RO.ts

QT_LRELEASE = $$QMAKE_MOC
QT_LRELEASE ~= s,moc,lrelease,

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$QT_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
