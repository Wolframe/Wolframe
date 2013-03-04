/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#include "NetworkDataLoader.hpp"
#include "WolframeClientProtocol.hpp"
#include <QDebug>
#include <QByteArray>
#include <QXmlStreamWriter>

NetworkDataLoader::NetworkDataLoader( WolframeClient *_wolframeClient, bool _debug )
	: m_wolframeClient( _wolframeClient ),
	  m_map( new QHash<QByteArray, QPair<QString, QString> >( ) ),
	  m_debug( _debug )
{
	connect( m_wolframeClient, SIGNAL( answerReceived( bool,const QByteArray&,const QByteArray&) ),
		this, SLOT( gotAnswer( bool,const QByteArray&,const QByteArray&) ) );
	connect( m_wolframeClient, SIGNAL( error( QString ) ),
		this, SLOT( gotError( QString ) ) );
}

void NetworkDataLoader::request( QString windowName, QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "NETWORK REQUEST" << windowName << formName << widgetName;
	
	if( !props->contains( "doctype" ) ) {
		qCritical( ) << "FATAL: request impossible without 'doctype' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "rootelement" ) ) {
		qCritical( ) << "FATAL: request impossible without 'rootelement' for form " << formName << " and widget " << widgetName;
		return;
	}
	
	if( !props->contains( "action" ) )
	{
		qDebug( ) << "handle no action request";
		handleRequest( windowName, formName, widgetName, xml, props );
		return;
	}

	QString action = props->value( "action" );
	if( action == "create" ) {
		handleCreate( windowName, formName, xml, props );
	} else if( action == "read" ) {
		if( widgetName.isEmpty( ) ) {
			handleRead( windowName, formName, props );
		} else {
			handleDomainDataLoad( windowName, formName, widgetName, props );
		}
	} else if( action == "update" ) {
		handleUpdate( windowName, formName, xml, props );
	} else if( action == "delete" ) {
		handleDelete( windowName, formName, props );
	} else {
		qCritical( ) << "ERROR: unknown action " << action << " for window" << windowName << "for form " << formName << " and widget " << widgetName;
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

void NetworkDataLoader::handleCreate( QString windowName, QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
// what doctype do we expect in the answer?
	QString docType = props->value( "doctype" );
	QString action = props->value( "action" );
	QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());

	m_map->insert( tag, qMakePair( name, QString("#") + windowName) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	
	m_wolframeClient->request( tag, xml );
}

void NetworkDataLoader::handleRead( QString windowName, QString name, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );
	
// pretty-printing only in debug mode (because of superfluous
// white spaces sent to server)
	if( m_debug ) {
		xml.setAutoFormatting( true );
		xml.setAutoFormattingIndent( 2 );
	}

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );
	QStringList dataElements;
	if (props->contains( "dataelement")) {
		dataElements = props->value( "dataelement").split( ",");
	}

	if (dataElements.empty())
	{
		foreach( QString key, props->keys( ) ) {
// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
			if( key.startsWith( "_q_" ) ) continue;
// skip globals
			if( key.startsWith( "global." ) ) continue;
// ignore our own actions
			if( key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state") continue;
			xml.writeAttribute( key, props->value( key ) );
		}
	}
	else
	{
		foreach( QString key, dataElements) {
			if (props->contains( key))
			{
				xml.writeAttribute( key, props->value( key ) );
			}
			else
			{
				qWarning( ) << "dataelement property" << key << "undefined";
				xml.writeAttribute( key, "");
			}
		}
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());
	m_map->insert( tag, qMakePair( name, QString("#") + windowName) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( tag, data );

}

void NetworkDataLoader::handleUpdate( QString windowName, QString name, QByteArray xml, QHash<QString, QString> *props )
{
	qDebug( ) << "network request:\n" << xml;
	
	QString docType = props->value( "doctype" );

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());
	m_map->insert( tag, qMakePair( name, QString("#") + windowName) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( tag, xml );
}

void NetworkDataLoader::handleDelete( QString windowName, QString name, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );

// pretty-printing only in debug mode (because of superfluous
// white spaces sent to server)
	if( m_debug ) {
		xml.setAutoFormatting( true );
		xml.setAutoFormattingIndent( 2 );
	}

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );

	foreach( QString key, props->keys( ) ) {
// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
		if( key.startsWith( "_q_" ) ) continue;
// skip globals
		if( key.startsWith( "global." ) ) continue;
// ignore our own actions
		if( key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state" ) continue;
		xml.writeAttribute( key, props->value( key ) );
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "network request:\n" << data;

// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());

	m_map->insert( tag, qMakePair( name, QString("#") + windowName) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, false, docType );
	 
	m_wolframeClient->request( tag, data );
}

void NetworkDataLoader::handleDomainDataLoad( QString windowName, QString formName, QString widgetName, QHash<QString, QString> *props )
{
	QString rootElement = props->value( "rootelement" );
	QString docType = props->value( "doctype" );
	QByteArray data;
	QXmlStreamWriter xml( &data );

// pretty-printing only in debug mode (because of superfluous
// white spaces sent to server)
	if( m_debug ) {
		xml.setAutoFormatting( true );
		xml.setAutoFormattingIndent( 2 );
	}

	xml.writeStartDocument( );
	xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
	xml.writeStartElement( rootElement );
	QStringList dataElements;
	if (props->contains( "dataelement")) {
		dataElements = props->value( "dataelement").split( ",");
	}

	if (dataElements.empty())
	{
		foreach( QString key, props->keys( ) ) {
	// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
			if( key.startsWith( "_q_" ) ) continue;
	// skip globals
			if( key.startsWith( "global." ) ) continue;
	// ignore our own actions
			if( key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state") continue;
			xml.writeAttribute( key, props->value( key ) );
		}
	// assuming the root element has always id 1
		xml.writeAttribute( "id", "1" );
	}
	else
	{
		foreach( QString key, dataElements) {
			if (props->contains( key))
			{
				xml.writeAttribute( key, props->value( key ) );
			}
			else
			{
				qWarning( ) << "dataelement property" << key << "undefined";
				xml.writeAttribute( key, "");
			}
		}
	}
	xml.writeEndElement( );
	xml.writeEndDocument( );

	qDebug( ) << "domain data load network request"<< formName << widgetName << ":\n" << data;
	
// what doctype do we expect in the answer?
	QString action = props->value( "action" );
	QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());
	m_map->insert( tag, qMakePair( formName, widgetName + "#" + windowName) );

	qDebug( ) << "MAP:" << docType << action << mapDoctype( action, true, docType );
	 
	m_wolframeClient->request( tag, data );
}

void NetworkDataLoader::handleRequest( QString windowName, QString formName, QString widgetName, const QByteArray& data, QHash<QString, QString> *props )
{
	QString docType = props->value( "doctype" );

// the data handler didn't compose the request yet: do it here. This is the
// case for domain reads (hack)	
	if( data.isEmpty( ) ) {
		QString rootElement = props->value( "rootelement" );
		QByteArray buffer;
		QXmlStreamWriter xml( &buffer );

// pretty-printing only in debug mode (because of superfluous
// white spaces sent to server)
		if( m_debug ) {
			xml.setAutoFormatting( true );
			xml.setAutoFormattingIndent( 2 );
		}

		xml.writeStartDocument( );
		xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" ).arg( rootElement ).arg( docType ) );
		xml.writeStartElement( rootElement );
		QStringList dataElements;
		if (props->contains( "dataelement")) {
			dataElements = props->value( "dataelement").split( ",");
		}

		if (dataElements.empty())
		{
			foreach( QString key, props->keys( ) ) {
// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
				if( key.startsWith( "_q_" ) ) continue;
// skip globals
				if( key.startsWith( "global." ) ) continue;
// ignore our own actions
				if( key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state" ) continue;
				xml.writeAttribute( key, props->value( key ) );
			}
			xml.writeAttribute( "id", "1" );
			// assuming the root element has always id, was used for trees in configurator, deemed
			// deprecated as we get the whole domain or we'll pass a parameter like 'id', 'search'
			// explicitly
		}
		else
		{
			foreach( QString key, dataElements) {
				if (props->contains( key))
				{
					xml.writeAttribute( key, props->value( key ) );
				}
				else
				{
					qWarning( ) << "dataelement property" << key << "undefined";
					xml.writeAttribute( key, "");
				}
			}
		}
		xml.writeEndElement( );
		xml.writeEndDocument( );
		
		qDebug( ) << "new style network request"<< formName << widgetName << ":\n" << data;

	// what doctype do we expect in the answer?
		QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());
		m_map->insert( tag, qMakePair( formName, widgetName + "#" + windowName) );

		qDebug( ) << "MAP:" << docType << "new style request map";

		m_wolframeClient->request( tag, buffer);
	}
	else
	{
		qDebug( ) << "new style network request"<< formName << widgetName << ":\n" << data;

	// what doctype do we expect in the answer?
		QByteArray tag = WolframeClientProtocol::requestUID( docType.toAscii());
		m_map->insert( tag, qMakePair( formName, widgetName + "#" + windowName) );

		qDebug( ) << "MAP:" << docType << "new style request map";

		m_wolframeClient->request( tag, data );
	}
}

void NetworkDataLoader::gotAnswer( bool success, const QByteArray& tag, const QByteArray& content )
{
	if( !success ) {
		qCritical( ) << "ERROR: " << tag << content;
		return;
	}
		
	if( !m_map->contains( tag ) ) {
		qCritical( ) << "ERROR: answer for unknown request of " << tag << m_map;
		return;
	}
	
	QPair<QString, QString> pair = m_map->value( tag );
	QStringList addr = pair.second.split( "#");
	QString formName = pair.first;
	QString widgetName = addr[0];
	QString windowName = addr[1];

	m_map->remove( tag );

	qDebug( ) << "ANSWER for window " << windowName << "form" << formName << "and widget" << widgetName;
	
	emit answer( formName, widgetName, content);
}

void NetworkDataLoader::gotError( QString error )
{
	qCritical( ) << "ERROR: error in network data loader" << error;
}
