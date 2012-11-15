//
// SqliteDataLoader.cpp
//

#include "SqliteDataLoader.hpp"

#include <QDir>
#include <QFile>


SqliteDataLoader::SqliteDataLoader( QString dbName )
	: m_dbName( dbName ) 
{
}

void SqliteDataLoader::request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props )
{
}
