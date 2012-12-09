//
// FileFormLoader.cpp
//

#include "FileFormLoader.hpp"
#include "global.hpp"

#include <QDir>
#include <QFile>
#include <QResource>

#include <QDebug>


FileFormLoader::FileFormLoader( QString formDir, QString localeDir, QString resourcesDir )
	: m_formDir( formDir ), m_localeDir( localeDir ), m_resourcesDir( resourcesDir )
{
	initialize( );
}

void FileFormLoader::initialize( )
{
	// register all resources in the resources form dir to the application
	QDir resourcesDir( m_resourcesDir );
	QStringList nameFilters;
	nameFilters << "*.rcc";
	resourcesDir.setNameFilters( nameFilters );
	QFileInfoList resources = resourcesDir.entryInfoList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name );
	foreach( QFileInfo resource, resources ) {
		QString filename = resource.filePath( );
		qWarning( ) << "Loading resource file" << filename;
		if( !QResource::registerResource( filename ) ) {
			qWarning( ) << "Unable to open resource file" << filename;
		}
	}
}

void FileFormLoader::initiateListLoad( )
{
	QDir formsDir( m_formDir );
	QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		.replaceInStrings( ".ui", "" );
	emit formListLoaded( forms );
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


void FileFormLoader::initiateGetLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( DEFAULT_LOCALE ); // default locale, always around
	
// read list of supported languages for all forms based on their qm files available
	QDir translationDir( m_localeDir );
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
	
	languageCodes.removeDuplicates( );

	emit languageCodesLoaded( languageCodes );
}

void FileFormLoader::initiateFormSave( QString name, QByteArray form )
{
}

void FileFormLoader::initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin )
{
}
			
void FileFormLoader::initiateFormDelete( QString name )
{
}

void FileFormLoader::initiateFormLocalizationDelete( QString name, QLocale locale )
{
}

