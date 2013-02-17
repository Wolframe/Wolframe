HEADERS       = \
    mdiChild.hpp \
    mainWindow.hpp \
    connection.hpp \
    settings.hpp \
    manageServersDialog.hpp \
    loginDialog.hpp \
    serverDefinitionDialog.hpp \
    preferencesDialog.hpp
SOURCES       = main.cpp \
    mainWindow.cpp \
    mdiChild.cpp \
    connection.cpp \
    settings.cpp \
    manageServersDialog.cpp \
    loginDialog.cpp \
    serverDefinitionDialog.cpp \
    preferencesDialog.cpp
RESOURCES     = qWolfClient.qrc

FORMS += \
    manageServersDialog.ui \
    loginDialog.ui \
    serverDefinitionDialog.ui \
    preferencesDialog.ui

OTHER_FILES += \
    .gitignore
