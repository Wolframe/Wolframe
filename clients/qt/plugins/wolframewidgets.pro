TEMPLATE    = lib
TARGET      = wolframewidgets
CONFIG     += qt warn_on plugin
QT         += 
INCLUDEPATH += .. filechooser formwidget

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
SOURCES += \
	WolframeWidgets.cpp \
	filechooser/FileChooserPlugin.cpp \
	../FileChooser.cpp
#	formwidget/FormWidgetPlugin.cpp \
#	../FormWidget.cpp

HEADERS += \
	WolframeWidgets.hpp \
	filechooser/FileChooserPlugin.hpp \
	../FileChooser.hpp
#	formwidget/FormWidgetPlugin.hpp \
#	../FormWidget.hpp
