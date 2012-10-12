//
// NetworkDataLoader.cpp
//

#include "NetworkDataLoader.hpp"

#include <QDir>
#include <QFile>

namespace _Wolframe {
	namespace QtClient {

NetworkDataLoader::NetworkDataLoader( QString dir ) : m_dir( dir ) 
{
}

void NetworkDataLoader::initiateDataLoad( QString name )
{
// read directly here and stuff data into the signal
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );	
	emit dataLoaded( name, data );
}

void NetworkDataLoader::initiateDataSave( QString name, QByteArray data )
{
	QFile file( m_dir + "/" + name + ".xml" );
	file.open( QFile::WriteOnly );
	file.write( data );
	file.close( );
	emit dataSaved( name );
}

void NetworkDataLoader::initiateDomainDataLoad( QString form_name, QString widget_name )
{
	QFile file( m_dir + "/domain_" + form_name + "_" + widget_name + ".xml" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	emit domainDataLoaded( form_name, widget_name, data );
}

} // namespace QtClient
} // namespace _Wolframe
