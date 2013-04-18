TEMPLATE = lib

TARGET = wolframewidgets

CONFIG += qt warn_on plugin

QT += 

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

INCLUDEPATH += filechooser picturechooser

contains(QT_VERSION,^5\\..*) {
	QT += designer
} else {
	CONFIG += designer
}
	
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
	WolframeWidgets.cpp \
	filechooser/FileChooserPlugin.cpp \
	filechooser/FileChooser.cpp \
	picturechooser/PictureChooserPlugin.cpp \
	picturechooser/PictureChooser.cpp

HEADERS += \
	WolframeWidgets.hpp \
	filechooser/FileChooserPlugin.hpp \
	filechooser/FileChooser.hpp \
	picturechooser/PictureChooserPlugin.hpp \
	picturechooser/PictureChooser.hpp
