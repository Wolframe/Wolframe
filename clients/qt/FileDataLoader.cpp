//
// FileDataLoader.cpp
//

#include "FileDataLoader.hpp"

#include <QDir>
#include <QFile>


FileDataLoader::FileDataLoader( QString dir ) : m_dir( dir ) 
{
}

void FileDataLoader::initiateDataLoad( QString name )
{
// read directly here and stuff data into the signal
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );	
	emit dataLoaded( name, data );
}

void FileDataLoader::initiateDataSave( QString name, QByteArray data )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( data );
	file.close( );
	emit dataSaved( name );
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

