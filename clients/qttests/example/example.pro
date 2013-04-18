TEMPLATE = app

TARGET = example

CONFIG += qt debug

DEFINES += X_EXPORT=Q_DECL_IMPORT WITH_SSL=1

INCLUDEPATH += ../skeleton ../libqtwolframeclient

QT += core network

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

unix:LIBS += -L../libqtwolframeclient -lqtwolframeclient
win32:LIBS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

unix;PRE_TARGETDEPS += ../libqtwolframeclient/libqtwolframeclient.so
win32:PRE_TARGETDEPS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

SOURCES += \
	main.cpp \
	MainWindow.cpp \
	../skeleton/SkeletonMainWindow.cpp \
	../skeleton/loginDialog.cpp \
	../skeleton/manageServersDialog.cpp \
	../skeleton/serverDefinitionDialog.cpp
	
HEADERS += \
	MainWindow.hpp \
	../skeleton/SkeletonMainWindow.hpp \
	../skeleton/loginDialog.hpp \
	../skeleton/manageServersDialog.hpp \
	../skeleton/serverDefinitionDialog.hpp

FORMS += \
	MainWindow.ui

RESOURCES = \
	example.qrc

