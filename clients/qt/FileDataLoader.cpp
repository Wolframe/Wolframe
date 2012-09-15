//
// FileDataLoader.cpp
//

#include "FileDataLoader.hpp"

#include <QDir>
#include <QFile>

namespace _Wolframe {
	namespace QtClient {

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

} // namespace QtClient
} // namespace _Wolframe
