//
// NetworkDataLoader.cpp
//

#include "NetworkDataLoader.hpp"

#include <QDir>
#include <QFile>


NetworkDataLoader::NetworkDataLoader( WolframeClient *_wolframeClient )
	: m_wolframeClient( _wolframeClient )
{
}

void NetworkDataLoader::request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props )
{
}

