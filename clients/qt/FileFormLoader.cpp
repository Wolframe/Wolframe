//
// FileFormLoader.cpp
//

#include "FileFormLoader.hpp"

#include <QDir>
#include <QFile>

#include <iostream>

namespace _SMERP {
	namespace QtClient {

FileFormLoader::FileFormLoader( QString dir ) : m_dir( dir ) 
{
}

void FileFormLoader::initiateListLoad( )
{
// doing nothing here, deferred to getFormNames on demand
	emit formListLoaded( );
}

void FileFormLoader::initiateFormLoad( QString &name )
{
// read directly here and stuff data into the signal
	QFile file( m_dir + "/" + name );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	std::cout << "Read form '" << file.fileName( ).toStdString( ) << "' with " << data.size( ) << " octects." << std::endl;
	
	emit formLoaded( name, data );
}

QStringList FileFormLoader::getFormNames( )
{
// read list of forms diretcory on demand
	QDir forms_dir( QLatin1String( "forms" ) );
	QStringList forms = forms_dir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name );
	return forms;
}

QString FileFormLoader::getFormRef( QString &name )
{
}

} // namespace QtClient
} // namespace _SMERP
