//
// FileDataLoader.cpp
//

#include "FileDataLoader.hpp"

#include <QDir>
#include <QFile>

FileDataLoader::FileDataLoader( QString dir ) : m_dir( dir ) 
{
}

void FileDataLoader::initiateDataCreate( QString name, QByteArray data )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( data );
	file.close( );
	emit dataCreated( name );
}

void FileDataLoader::initiateDataRead( QString name )
{
// read directly here and stuff data into the signal
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );	
	emit dataRead( name, data );
}

void FileDataLoader::initiateDataUpdate( QString name, QByteArray data )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( data );
	file.close( );
	emit dataUpdated( name );
}

void FileDataLoader::initiateDataDelete( QString name )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.remove( );	
	emit dataDeleted( name );
}

void FileDataLoader::initiateDomainDataLoad( QString form_name, QString widget_name )
{
	QFile file( m_dir + "/domain_" + form_name + "_" + widget_name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	emit domainDataLoaded( form_name, widget_name, data );
}

