//
// FileFormLoader.cpp
//

#include "FileFormLoader.hpp"

#include <QDir>
#include <QFile>

namespace _Wolframe {
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
	QFile file( m_dir + "/" + name + ".ui" );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );	
	emit formLoaded( name, data );
}

QStringList FileFormLoader::getFormNames( )
{
// read list of forms diretcory on demand
	QDir forms_dir( QLatin1String( "forms" ) );
	QStringList forms = forms_dir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		.replaceInStrings( ".ui", "" );
	return forms;
}

} // namespace QtClient
} // namespace _Wolframe
