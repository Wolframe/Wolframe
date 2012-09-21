//
// FileFormLoader.cpp
//

#include "FileFormLoader.hpp"

#include <QDir>
#include <QFile>

#include <QDebug>

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
	qDebug( ) << "Reading file " << name;
	QFile file( name );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	return data;
}

void FileFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	QByteArray localization = readFile( m_localeDir + "/" + name + "." + locale.name( ) + ".qm" );
	
	emit formLocalizationLoaded( name, localization );
}

void FileFormLoader::initiateFormLoad( QString &name )
{
// read directly here and stuff data into the signal
	QByteArray form = readFile( m_formDir + "/" + name + ".ui" );

	emit formLoaded( name, form );
}

QStringList FileFormLoader::getFormNames( )
{
// read list of forms directory on demand
	QDir formsDir( QLatin1String( "forms" ) );
	QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		.replaceInStrings( ".ui", "" );
	return forms;
}

QStringList FileFormLoader::getLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( "en_US" ); // default locale, always around
	
// read list of supperted languages for all forms based on their qm files available
	QDir translationDir( QLatin1String( "i18n" ) );
	translationDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
	translationDir.setSorting( QDir::Name );
	QStringList filters;
	filters << "*.qm";
	translationDir.setNameFilters( filters );
	QStringList localeFiles = translationDir.entryList( );
	QMutableStringListIterator it( localeFiles );
	while( it.hasNext( ) ) {
		it.next( );
		QStringList parts = it.value( ).split( "." );
		languageCodes.push_back( parts[1] );		
	}

	return languageCodes;
}

} // namespace QtClient
} // namespace _Wolframe
