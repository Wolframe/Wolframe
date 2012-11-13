TEMPLATE    = lib
TARGET      = wolframefilechooser
CONFIG     += qt warn_on plugin
QT         += 
INCLUDEPATH += ../..

CONFIG += designer
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
SOURCES += FileChooserPlugin.cpp ../../FileChooser.cpp
HEADERS += FileChooserPlugin.hpp ../../FileChooser.hpp
