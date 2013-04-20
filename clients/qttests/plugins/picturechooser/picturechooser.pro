TEMPLATE = lib

TARGET = picturechooser

CONFIG += qt warn_on plugin

INCLUDEPATH += ../filechooser

QT += 

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

contains(QT_VERSION,^5\\..*) {
	QT += designer
} else {
	CONFIG += designer
}
	
#unix:LIBS += -L../filechooser -lfilechooser
#win32:LIBS += ../filechooser/debug/filechooser.lib

#unix;PRE_TARGETDEPS += ../filechooser/filechooser.so
#win32:PRE_TARGETDEPS += ../filechooser/debug/filechooser.lib

win32|mac: CONFIG+= debug_and_release
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/designer
contains(TEMPLATE, ".*lib"):TARGET = $$qtLibraryTarget($$TARGET)

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# Input
SOURCES += \
	PictureChooserPlugin.cpp \
	PictureChooser.cpp \
	../filechooser/FileChooser.cpp

HEADERS += \
	PictureChooserPlugin.hpp \
	PictureChooser.hpp \
	../filechooser/FileChooser.hpp
