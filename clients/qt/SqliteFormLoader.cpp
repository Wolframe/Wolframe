//
// SqliteFormLoader.cpp
//

#include "SqliteFormLoader.hpp"

#include <QDir>
#include <QFile>

#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

namespace _Wolframe {
	namespace QtClient {

SqliteFormLoader::SqliteFormLoader( QString dbName )
	: m_dbName( dbName )
{
	initialize( );
}

void SqliteFormLoader::initialize( )
{
	Version v = getSchemaVersion( );
	if( v.valid( ) ) {
		qDebug( ) << "version of schema is " << v.toString( );
	} else {
		qDebug( ) << "No version info found in database, creating schema now";
		QSqlDatabase db = QSqlDatabase::database( m_dbName );
		
		QSqlQuery qs( db );
		if( !qs.exec( "create table version( major int, minor int )" ) ) {
			qDebug( ) << "error when creating version table: " << qs.lastError( ).text( );
			return;
		}
		
		QSqlQuery q( "insert into version( major, minor ) values( :major, :minor )", db );
		q.bindValue( ":major", VERSION_MAJOR );
		q.bindValue( ":minor", VERSION_MINOR );
		if( !q.exec( ) ) {
			qDebug( ) << "error when inserting the version into the schema: " << qs.lastError( ).text( );
		}
	}
	
	v = getSchemaVersion( );
	if( !v.valid( ) ) {
		qDebug( ) << "no version info after creating it, funny!";
	}
}

Version SqliteFormLoader::getSchemaVersion( )
{
	QSqlQuery q( "select major,minor from version", QSqlDatabase::database( m_dbName ) );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int minorIdx = r.indexOf( "major" ); 
		int majorIdx = r.indexOf( "minor" );
		while( q.next( ) ) {
			return Version( q.value( minorIdx ).toInt( ), q.value( majorIdx ).toInt( ) );
		}
		return Version::invalidVersion( );
	} else {
		return Version::invalidVersion( );
	}
}

void SqliteFormLoader::initiateListLoad( )
{
	//~ QDir formsDir( QLatin1String( "forms" ) );
	//~ QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		//~ .replaceInStrings( ".ui", "" );
	//~ emit formListLoaded( forms );
}

void SqliteFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	//~ QByteArray localization = readFile( m_localeDir + "/" + name + "." + locale.name( ) + ".qm" );
	//~ 
	//~ emit formLocalizationLoaded( name, localization );
}

void SqliteFormLoader::initiateFormLoad( QString &name )
{
//~ // read directly here and stuff data into the signal
	//~ QByteArray form = readFile( m_formDir + "/" + name + ".ui" );
//~ 
	//~ emit formLoaded( name, form );
}


void SqliteFormLoader::initiateGetLanguageCodes( )
{
	//~ QStringList languageCodes;
	//~ languageCodes.push_back( "en_US" ); // default locale, always around
	//~ 
//~ // read list of supported languages for all forms based on their qm files available
	//~ QDir translationDir( QLatin1String( "i18n" ) );
	//~ translationDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
	//~ translationDir.setSorting( QDir::Name );
	//~ QStringList filters;
	//~ filters << "*.qm";
	//~ translationDir.setNameFilters( filters );
	//~ QStringList localeFiles = translationDir.entryList( );
	//~ QMutableStringListIterator it( localeFiles );
	//~ while( it.hasNext( ) ) {
		//~ it.next( );
		//~ QStringList parts = it.value( ).split( "." );
		//~ languageCodes.push_back( parts[1] );		
	//~ }
	//~ 
	//~ languageCodes.removeDuplicates( );
//~ 
	//~ emit languageCodesLoaded( languageCodes );
}

} // namespace QtClient
} // namespace _Wolframe
