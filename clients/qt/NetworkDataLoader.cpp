//
// NetworkDataLoader.cpp
//

#include "NetworkDataLoader.hpp"

#include <QDebug>
#include <QByteArray>
#include <QXmlStreamWriter>

NetworkDataLoader::NetworkDataLoader( WolframeClient *_wolframeClient )
	: m_wolframeClient( _wolframeClient ),
	  m_map( new QHash<QString, QPair<QString, QString> >( ) )
{
	connect( m_wolframeClient, SIGNAL( answerReceived( QStringList, QString ) ),
		this, SLOT( gotAnswer( QStringList, QString ) ) );
	connect( m_wolframeClient, SIGNAL( error( QString ) ),
		this, SLOT( gotError( QString ) ) );
}

void NetworkDataLoader::request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props )
{
	if( !props->contains( "doctype" ) ) {
		qCritical( ) << "FATAL: request impossible without 'doctype' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "rootelement" ) ) {
		qCritical( ) << "FATAL: request impossible without 'rootelement' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "action" ) ) {
		qCritical( ) << "FATAL: request impossible without action for form " << formName << " and widget " << widgetName;
		return;
	}

	QString action = props->value( "action" );
	if( action == "create" ) {
		handleCreate( formName, xml, props );
	} else if( action == "read" ) {
		if( widgetName.isEmpty( ) ) {
			handleRead( formName, props );
		} else {
			handleDomainDataLoad( formName, widgetName, props );
		}
	} else if( action == "update" ) {
		handleUpdate( formName, xml, props );
	} else if( action == "delete" ) {
		handleDelete( formName, props );
	} else {
		qCritical( ) << "ERROR: unknown action for for form " << formName << " and widget " << widgetName;
		return;
	}	
}

QString NetworkDataLoader::mapAction( QString action )
{
	if( action == "create" ) {
		return "create";
	} else if( action == "read" ) {
		return "";
	} else if( action == "update" ) {
		return "edit";
	} else if( action == "delete" ) {
		return "delete";
	}
	
	return QString( );
}

QString NetworkDataLoader::mapDoctype( QString action, bool domain, QString docType )
{
// the doctype has also a ".simpleform" which doesn't come back?
	QStringList p = docType.split( "." );
	
	if( action == "create" ) {
		return QString( "create%1" ).arg( p[0] );
	} else if( action == "read" ) {
		if( domain ) {
			return p[0];
		} else {
			return p[0];
		}
	} else if( action == "update" ) {
		return QString( "edit%1" ).arg( p[0] );
	} else if( action == "delete" ) {
		return QString( "delete%1" ).arg( p[0] );
	}
	
	return QString( );
}

void NetworkDataLoader::handleCreate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
// what doctype do we expect in the answer?
	QString docType = props->value( "doctype" );
	QString action = props->value( "action" );
	m_map->insert( mapDoctype( action, false, docType ), qMakePair( name, QString( ) ) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	
	m_wolframeClient->request( mapAction( action ), xml );
}

void NetworkDataLoader::handleRead( QString name, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );
	xml.setAutoFormatting( true );
	xml.setAutoFormattingIndent( 2 );

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );
	foreach( QString key, props->keys( ) ) {
// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
		if( key.startsWith( "_q_" ) ) continue;
// skip globals
		if( key.startsWith( "global." ) ) continue;
// ignore our own actions
		if( key == "doctype" || key == "rootelement" || key == "action" || key == "initAction" ) continue;
		xml.writeAttribute( key, props->value( key ) );
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	m_map->insert( mapDoctype( action, false, docType ), qMakePair( name, QString( ) ) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( mapAction( action ), data );

}

void NetworkDataLoader::handleUpdate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
	QString docType = props->value( "doctype" );

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	m_map->insert( mapDoctype( action, false, docType ), qMakePair( name, QString( ) ) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( mapAction( action ), xml );
}

void NetworkDataLoader::handleDelete( QString name, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );
	xml.setAutoFormatting( true );
	xml.setAutoFormattingIndent( 2 );

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );
	foreach( QString key, props->keys( ) ) {
// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
		if( key.startsWith( "_q_" ) ) continue;
// skip globals
		if( key.startsWith( "global." ) ) continue;
// ignore our own actions
		if( key == "doctype" || key == "rootelement" || key == "action" || key == "initAction" ) continue;
		xml.writeAttribute( key, props->value( key ) );
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	m_map->insert( mapDoctype( action, false, docType ), qMakePair( name, QString( ) ) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( mapAction( action ), data );
}

void NetworkDataLoader::handleDomainDataLoad( QString formName, QString widgetName, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );
	xml.setAutoFormatting( true );
	xml.setAutoFormattingIndent( 2 );

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );
// assuming the root element has always id 1
	xml.writeAttribute( "id", "1" );
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "domain data load network request"<< formName << widgetName << ":\n" << data;
	
// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	m_map->insert( mapDoctype( action, true, docType ), qMakePair( formName, widgetName ) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, true, docType );
	 
	m_wolframeClient->request( mapAction( action ), data );
}

void NetworkDataLoader::gotAnswer( QStringList params, QString content )
{
	if( params.size( ) < 2 ) {
		qCritical( ) << "ERROR: got an empty parameter list for a command, can't link it to form and widget!";
		return;
	}
	
// TODO: how can we tag requests in the protocol, so we actually get back the right
// data??! Now we get 'REQUEST doctype', for now, push the form and widget into a
// hash and receive it by doctype
	QString docType = params[1];

	qDebug( ) << "OK: answer in network data loader, params:" << params << "\ncontent:\n" << content;
	
	if( !m_map->contains( docType ) ) {
		qCritical( ) << "ERROR: answer for unknown request of doctype" << docType << m_map;
		return;
	}
	
	QPair<QString, QString> pair = m_map->value( docType );
	QString formName = pair.first;
	QString widgetName = pair.second;
	
	m_map->remove( docType );
	
	qDebug( ) << "ANSWER for form" << formName << "and widget" << widgetName;
	
	emit answer( formName, widgetName, content.toUtf8( ) );
}

void NetworkDataLoader::gotError( QString error )
{
	qCritical( ) << "ERROR: error in network data loader" << error;
}
