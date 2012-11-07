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
// check version of schema, create schema if necessary
	Version v = getSchemaVersion( );
	if( v.valid( ) ) {
		qDebug( ) << "version of schema is " << v.toString( );
	} else {
		qDebug( ) << "No version info found in database, creating schema now";
		QSqlDatabase db = QSqlDatabase::database( m_dbName );

// schema creation		
		QSqlQuery qs( db );
		if( !qs.exec( "create table version( version text )" ) ) {
			qDebug( ) << "error when creating version table: " << qs.lastError( ).text( );
			return;
		}

		QSqlQuery q( "insert into version( version ) values( :version )", db );
		v = Version( VERSION_MAJOR, VERSION_MINOR );
		q.bindValue( ":version", v.toString( ) );
		if( !q.exec( ) ) {
			qDebug( ) << "error when inserting the version into the schema: " << qs.lastError( ).text( );
		}

		if( !qs.exec( "create table uiform( id int, name text, version text, source blob )" ) ) {
			qDebug( ) << "error when creating uiform table: " << qs.lastError( ).text( );
			return;
		}

		if( !qs.exec( "create table uitranslation( form_id int, language text, version text, source blob, binary blob )" ) ) {
			qDebug( ) << "error when creating uitranslation table: " << qs.lastError( ).text( );
			return;
		}

		if( !qs.exec( "create table uistylesheet( form_id int, version text, source blob )" ) ) {
			qDebug( ) << "error when creating uistylesheet table: " << qs.lastError( ).text( );
			return;
		}	
	}

// compare versions of schema	
	v = getSchemaVersion( );
	if( !v.valid( ) ) {
		qDebug( ) << "no version info after creating it, funny!";
	}
	if( v > Version( VERSION_MAJOR, VERSION_MINOR ) ) {
		if( v.getMajor( ) == VERSION_MAJOR ) {
			qDebug( ) << "Version " << v.toString( ) << " runs in backward compatibility mode";
		} else {
			qDebug( ) << "Schema in database newer than the one in the code. Can't run!";
		}
	}
}

Version SqliteFormLoader::getSchemaVersion( )
{
	QSqlQuery q( "select version from version", QSqlDatabase::database( m_dbName ) );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int versionIdx = r.indexOf( "version" ); 
		while( q.next( ) ) {
			return Version( q.value( versionIdx ).toString( ) );
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
