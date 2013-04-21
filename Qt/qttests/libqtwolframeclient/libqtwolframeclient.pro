TEMPLATE = lib

TARGET = qtwolframeclient

VERSION = 0.0.1

CONFIG += qt debug static_and_shared

DEFINES += X_EXPORT=Q_DECL_EXPORT WITH_SSL=1

QT += core network

contains(QT_VERSION,^5\\..*) {
QT += widgets
}

SOURCES += \
	connection.cpp \
	WolframeClient.cpp \
	WolframeClientProtocolBase.cpp \
	WolframeClientProtocol.cpp

HEADERS += \
	connection.hpp \
	WolframeClient.hpp \
	WolframeClientProtocolBase.hpp \
	WolframeClientProtocol.hpp
