//
// FileFormLoader.cpp
//

#include "FileFormLoader.hpp"

#include <QDir>
#include <QFile>

namespace _Wolframe {
	namespace QtClient {

FileFormLoader::FileFormLoader( QString formDir, QString localeDir )
	: m_formDir( formDir ), m_localeDir( localeDir ) 
{
}

void FileFormLoader::initiateListLoad( )
{
// doing nothing here, deferred to getFormNames on demand
	emit formListLoaded( );
}

QByteArray FileFormLoader::readFile( QString name )
{
	QFile file( name );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	return data;
}

void FileFormLoader::initiateFormLoad( QString &name, QLocale locale )
{
// read directly here and stuff data into the signal
	QByteArray form = readFile( m_formDir + "/" + name + ".ui" );
	QByteArray localization = readFile( m_localeDir + "/" + name + "." + locale.name( ) + ".qm" );

	emit formLoaded( name, form, localization );
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
