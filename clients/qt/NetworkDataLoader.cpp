//
// NetworkDataLoader.cpp
//

#include "NetworkDataLoader.hpp"

#include <QDir>
#include <QFile>


NetworkDataLoader::NetworkDataLoader( WolframeClient *_wolframeClient )
	: m_wolframeClient( _wolframeClient )
{
}

void NetworkDataLoader::initiateDataLoad( QString name )
{
	//~ QString s = m_wolframeClient->syncRun( "getData " + name );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble loading form data for form" << name;
	//~ } else {
		//~ emit dataLoaded( name, s.toUtf8( ) );
	//~ }
}

void NetworkDataLoader::initiateDataSave( QString name, QByteArray data )
{
	//~ QString s = m_wolframeClient->syncRun( "saveData " + name, QString( data ) );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble saving form data for form" << name;
	//~ } else {
		//~ emit dataSaved( name );
	//~ }
}

void NetworkDataLoader::initiateDataDelete( QString name )
{
	//~ emit dataDeleted( name );
}

void NetworkDataLoader::initiateDomainDataLoad( QString form_name, QString widget_name )
{
	//~ QString s = m_wolframeClient->syncRun( "getDomain " + form_name + " " + widget_name );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble loading domain data for form" << form_name << "and widget" << widget_name;
	//~ } else {
		//~ emit domainDataLoaded( form_name, widget_name, s.toUtf8( ) );
	//~ }
}

