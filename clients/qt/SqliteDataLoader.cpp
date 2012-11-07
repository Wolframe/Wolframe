//
// SqliteDataLoader.cpp
//

#include "SqliteDataLoader.hpp"

#include <QDir>
#include <QFile>

namespace _Wolframe {
	namespace QtClient {

SqliteDataLoader::SqliteDataLoader( QString dbName )
	: m_dbName( dbName ) 
{
}

void SqliteDataLoader::initiateDataLoad( QString name )
{
//~ // read directly here and stuff data into the signal
	//~ QFile file( m_dir + "/" + name + ".xml" );
	//~ file.open( QFile::ReadOnly );
	//~ QByteArray data = file.readAll( );
	//~ file.close( );	
	//~ emit dataLoaded( name, data );
}

void SqliteDataLoader::initiateDataSave( QString name, QByteArray data )
{
	//~ QFile file( m_dir + "/" + name + ".xml" );
	//~ file.open( QFile::WriteOnly );
	//~ file.write( data );
	//~ file.close( );
	//~ emit dataSaved( name );
}

void SqliteDataLoader::initiateDomainDataLoad( QString form_name, QString widget_name )
{
	//~ QFile file( m_dir + "/domain_" + form_name + "_" + widget_name + ".xml" );
	//~ file.open( QFile::ReadOnly );
	//~ QByteArray data = file.readAll( );
	//~ file.close( );
	//~ emit domainDataLoaded( form_name, widget_name, data );
}

} // namespace QtClient
} // namespace _Wolframe
