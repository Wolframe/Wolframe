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

void SqliteDataLoader::initiateDataCreate( QString name, QByteArray data )
{
	//~ QFile file( m_dir + "/" + name + ".xml" );
	//~ file.open( QFile::WriteOnly );
	//~ file.write( data );
	//~ file.close( );
	//~ emit dataCreated( name );
}

void SqliteDataLoader::initiateDataRead( QString name )
{
//~ // read directly here and stuff data into the signal
	//~ QFile file( m_dir + "/" + name + ".xml" );
	//~ file.open( QFile::ReadOnly );
	//~ QByteArray data = file.readAll( );
	//~ file.close( );	
	//~ emit dataRead( name, data );
}

void SqliteDataLoader::initiateDataUpdate( QString name, QByteArray data )
{
	//~ QFile file( m_dir + "/" + name + ".xml" );
	//~ file.open( QFile::WriteOnly );
	//~ file.write( data );
	//~ file.close( );
	//~ emit dataUpdated( name );
}

void SqliteDataLoader::initiateDataDelete( QString name )
{
	//~ emit dataDeleted( name );
}

void SqliteDataLoader::initiateDomainDataLoad( QString form_name, QString widget_name )
{
	//~ QFile file( m_dir + "/domain_" + form_name + "_" + widget_name + ".xml" );
	//~ file.open( QFile::ReadOnly );
	//~ QByteArray data = file.readAll( );
	//~ file.close( );
	//~ emit domainDataLoaded( form_name, widget_name, data );
}

