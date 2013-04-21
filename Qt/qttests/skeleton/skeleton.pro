TEMPLATE = app

TARGET = skeleton

CONFIG += qt debug

DEFINES += X_EXPORT=Q_DECL_IMPORT WITH_SSL=1

QT += core network

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

unix:LIBS += -L../libqtwolframeclient -lqtwolframeclient
win32:LIBS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

INCLUDEPATH += ../libqtwolframeclient

unix:PRE_TARGETDEPS += ../libqtwolframeclient/libqtwolframeclient.so
win32:PRE_TARGETDEPS += ../libqtwolframeclient/debug/qtwolframeclient0.lib

SOURCES += \
	main.cpp \
	SkeletonMainWindow.cpp \
	loginDialog.cpp \
	manageServersDialog.cpp \
	serverDefinitionDialog.cpp
	
HEADERS += \
	SkeletonMainWindow.hpp \
	loginDialog.hpp \
	manageServersDialog.hpp \
	serverDefinitionDialog.hpp
	
FORMS += \
	SkeletonMainWindow.ui \
	loginDialog.ui \
	manageServersDialog.ui \
	serverDefinitionDialog.ui
	
RESOURCES = \
	skeleton.qrc

