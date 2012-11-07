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

		if( !qs.exec( "create table uiform( name text primary key, version text, source blob )" ) ) {
			qDebug( ) << "error when creating uiform table: " << qs.lastError( ).text( );
			return;
		}

		if( !qs.exec( "create table uitranslation( form_name text, locale text, version text, source blob, binary blob, foreign key( form_name ) references form( name ) )" ) ) {
			qDebug( ) << "error when creating uitranslation table: " << qs.lastError( ).text( );
			return;
		}

		if( !qs.exec( "create table uistylesheet( form_name text, version text, source blob, foreign key( form_name ) references form( name ) )" ) ) {
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
	QStringList forms;
	
	QSqlDatabase db = QSqlDatabase::database( m_dbName );
	QSqlQuery q( "select name from uiform", db );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int nameIdx = r.indexOf( "name" );
		while( q.next( ) ) {
			forms.append( q.value( nameIdx ).toString( ) );
		}
	}

	emit formListLoaded( forms );
}

void SqliteFormLoader::initiateFormLoad( QString &name )
{
	QSqlDatabase db = QSqlDatabase::database( m_dbName );
	QSqlQuery q( "select source from uiform where name=:name", db );
	q.bindValue( ":name", name );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int sourceIdx = r.indexOf( "source" );
		while( q.next( ) ) {
			QByteArray form = q.value( sourceIdx ).toByteArray( );
			emit formLoaded( name, form );
			return;
		}
		// more than one form with the same name?
	}
}

void SqliteFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	QSqlDatabase db = QSqlDatabase::database( m_dbName );
	QSqlQuery q( "select binary from uitranslation where name=:name and locale=:locale", db );
	q.bindValue( ":name", name );
	q.bindValue( ":locale", locale );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int binaryIdx = r.indexOf( "binary" );
		while( q.next( ) ) {
			QByteArray localization = q.value( binaryIdx ).toByteArray( );
			emit formLocalizationLoaded( name, localization );
			return;
		}
		// more than one localization per form/locale with the same name?
	}
}

void SqliteFormLoader::initiateGetLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( "en_US" ); // default locale, always around

	QSqlDatabase db = QSqlDatabase::database( m_dbName );
	QSqlQuery q( "select distinct( locale ) from uitranslation", db );
	if( q.exec( ) ) {
		QSqlRecord r = q.record( );
		int localeIdx = r.indexOf( "locale" );
		while( q.next( ) ) {
			languageCodes.append( q.value( localeIdx ).toString( ) );
		}
	}
	
	emit languageCodesLoaded( languageCodes );
}

} // namespace QtClient
} // namespace _Wolframe
