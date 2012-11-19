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
		qDebug( ) << "FATAL: request impossible without 'doctype' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "rootelement" ) ) {
		qDebug( ) << "FATAL: request impossible without 'rootelement' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "action" ) ) {
		qDebug( ) << "FATAL: request impossible without action for form " << formName << " and widget " << widgetName;
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
		qDebug( ) << "ERROR: unknonw action for for form " << formName << " and widget " << widgetName;
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

void NetworkDataLoader::handleCreate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
// the doctype has also a ".simpleform" which doesn't come back?
	QString docType = props->value( "doctype" );
	QStringList p = docType.split( "." );
	m_map->insert( p[0], qMakePair( name, QString( ) ) );
	 
	m_wolframeClient->request( mapAction( props->value( "action" ) ), xml );
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
// ignore our own actions
		if( key == "doctype" || key == "rootelement" || key == "action" || key == "initAction" ) continue;
		xml.writeAttribute( key, props->value( key ) );
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// the doctype has also a ".simpleform" which doesn't come back?
	QStringList p = docType.split( "." );
	m_map->insert( p[0], qMakePair( name, QString( ) ) );
	 
	m_wolframeClient->request( mapAction( props->value( "action" ) ), data );
}

void NetworkDataLoader::handleUpdate( QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
// the doctype has also a ".simpleform" which doesn't come back?
	QString docType = props->value( "doctype" );
	QStringList p = docType.split( "." );
	m_map->insert( p[0], qMakePair( name, QString( ) ) );
	 
	m_wolframeClient->request( mapAction( props->value( "action" ) ), xml );
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
// ignore our own actions
		if( key == "doctype" || key == "rootelement" || key == "action" || key == "initAction" ) continue;
		xml.writeAttribute( key, props->value( key ) );
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// the doctype has also a ".simpleform" which doesn't come back?
	QStringList p = docType.split( "." );
	m_map->insert( p[0], qMakePair( name, QString( ) ) );
	 
	m_wolframeClient->request( mapAction( props->value( "action" ) ), data );
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

	//qDebug( ) << "network request:\n" << data;
	
// the doctype has also a ".simpleform" which doesn't come back?
	QStringList p = docType.split( "." );
	m_map->insert( p[0], qMakePair( formName, widgetName ) );
	 
	m_wolframeClient->request( mapAction( props->value( "action" ) ), data );
}

void NetworkDataLoader::gotAnswer( QStringList params, QString content )
{
// TODO: how can we tag requests in the protocol, so we actually get back the right
// data??! Now we get 'REQUEST doctype', for now, push the form and widget into a
// hash and receive it by doctype
	QString docType = params[1];

	qDebug( ) << "OK: answer in network data loader, doctype:" << docType << ":\n"<< content;
	
	if( !m_map->contains( docType ) ) {
		qDebug( ) << "ERROR: answer for unknown request of doctype" << docType;
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
	qDebug( ) << "ERROR: error in network data loader" << error;
}
