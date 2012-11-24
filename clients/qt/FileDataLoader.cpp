//
// FileDataLoader.cpp
//

#include "FileDataLoader.hpp"

#include <QDir>
#include <QFile>
#include <QDebug>

FileDataLoader::FileDataLoader( QString dir ) : m_dir( dir ) 
{
}

void FileDataLoader::request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props )
{
	//qDebug( ) << "Request" << formName << widgetName << *props;
	
// nothing to do	
	if( !props->contains( "action" ) ) {
		return;
	}
	
// handle CRUD actions, they are file system dataloader specific
	QString action = props->value( "action" );
	if( action == "create" ) {
		handleCreate( formName, xml, props );
	} else if( action == "read" ) {
		if( widgetName.isEmpty( ) ) {
			handleRead( formName, props );
		} else {
			handleDomainDataLoad( formName, widgetName, props );
		}
	} else if( action == "update" ) {
		handleUpdate( formName, xml, props );
	} else if( action == "delete" ) {
		handleDelete( formName, props );
	}	
}

void FileDataLoader::handleCreate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( xml );
	file.close( );
	emit answer( name, QString( ), QByteArray( ) );
}

void FileDataLoader::handleRead( QString name, QHash<QString, QString> *props )
{
// read directly here and stuff data into the signal
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray xml = file.readAll( );
	file.close( );	
	emit answer( name, QString( ), xml );
}

void FileDataLoader::handleUpdate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( xml );
	file.close( );
	emit answer( name, QString( ), QByteArray( ) );
}

void FileDataLoader::handleDelete( QString name, QHash<QString, QString> *props )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.remove( );	
	emit answer( name, QString( ), QByteArray( ) );
}

void FileDataLoader::handleDomainDataLoad( QString formName, QString widgetName, QHash<QString, QString> *props )
{
	QFile file( m_dir + "/domain_" + formName + "_" + widgetName + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray xml = file.readAll( );
	file.close( );
	emit answer( formName, widgetName, xml );
}

