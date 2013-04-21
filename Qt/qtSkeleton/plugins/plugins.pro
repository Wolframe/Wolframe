CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(PictureChooserPlugin)
TEMPLATE    = lib

HEADERS     = \
    PictureChooserPlugin.hpp
SOURCES     = \
    PictureChooserPlugin.cpp
RESOURCES   = PictureChooserPlugin.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(PictureChooser.pri)
