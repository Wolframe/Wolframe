TEMPLATE = app

TARGET = qtclient

CONFIG += thread qt debug uitools

QT += core gui network

SOURCES += \
	MainWindow.cpp \
	FormLoader.cpp \
	FileFormLoader.cpp \
	NetworkFormLoader.cpp \
	DataLoader.cpp \
	FileDataLoader.cpp \
	NetworkDataLoader.cpp \
	DataHandler.cpp \
	DebugTerminal.cpp \
	HistoryLineEdit.cpp \
	WolframeClient.cpp \
	FormWidget.cpp \
	qcommandline.cpp \
	LoginDialog.cpp \
	qtclient.cpp

HEADERS += \
	DataHandler.hpp \
	DataLoader.hpp \
	FileDataLoader.hpp \
	FileFormLoader.hpp \
	DebugTerminal.hpp \
	FormLoader.hpp \
	FormWidget.hpp \
	HistoryLineEdit.hpp \
	LoginDialog.hpp \
	qcommandline.h \
	WolframeClient.hpp \
	MainWindow.hpp \
	NetworkDataLoader.hpp \
	NetworkFormLoader.hpp

TRANSLATIONS += \
	i18n/form1.de_CH.ts \
	i18n/form2.de_CH.ts \
	i18n/form3.de_CH.ts \
	themes/eeepc/qtclient.de_CH.ts \
	themes/phone/qtclient.de_CH.ts \
	themes/windows/qtclient.de_CH.ts

QT_LRELEASE = $$QMAKE_MOC 
QT_LRELEASE ~= s,moc,lrelease,

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$QT_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
