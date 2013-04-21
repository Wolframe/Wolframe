TEMPLATE = app

TARGET = qtclient

CONFIG += qt debug

DEFINES += X_EXPORT=Q_DECL_IMPORT WITH_SSL=1

QT += core network

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

unix:LIBS += -L../libqtwolframeclient -lqtwolframeclient
win32:LIBS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

unix:PRE_TARGETDEPS += ../libqtwolframeclient/libqtwolframeclient.so
win32:PRE_TARGETDEPS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

SOURCES += \
	main.cpp \
	Mainwindow.cpp
	
HEADERS += \
	MainWindow.hpp

RESOURCES = \
	qtclient.qrc

