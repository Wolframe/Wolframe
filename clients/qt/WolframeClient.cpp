/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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

#include "WolframeClient.hpp"

#include <QByteArray>
#include <QTcpSocket>
#include <QFile>
#include <QList>
#include <QTimer>

WolframeClient::WolframeClient( const ConnectionParameters _connParams,	QWidget *_parent )
	: m_connParams( _connParams ),
	m_state( Disconnected ),
	m_parent( _parent ),
	m_hasErrors( false ),
#ifdef WITH_SSL
	m_initializedSsl( false ),
#endif
	m_command( "CONNECT" ),
	m_queue( )
{
#ifdef WITH_SSL
	m_socket = new QSslSocket( this );
#else
	m_socket = new QTcpSocket( this );
#endif
	m_timeoutTimer = new QTimer( this );
	
	QObject::connect( m_timeoutTimer, SIGNAL( timeout( ) ),
		this, SLOT( timeoutOccurred( ) ) );

	QObject::connect( m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ),
		this, SLOT( error( QAbstractSocket::SocketError ) ) );
	QObject::connect( m_socket, SIGNAL( readyRead( ) ),
		this, SLOT( dataAvailable( ) ) );
	QObject::connect( m_socket, SIGNAL( connected( ) ),
		this, SLOT( privateConnected( ) ) );
	QObject::connect( m_socket, SIGNAL( disconnected( ) ),
		this, SLOT( privateDisconnected( ) ) );

#ifdef WITH_SSL
	QObject::connect( m_socket, SIGNAL( sslErrors( const QList<QSslError> & ) ),
		this, SLOT( sslErrors( const QList<QSslError> & ) ) );
	QObject::connect( m_socket, SIGNAL( encrypted( ) ),
		this, SLOT( encrypted( ) ) );
	QObject::connect( m_socket, SIGNAL( peerVerifyError( const QSslError & ) ),
		this, SLOT( peerVerifyError( const QSslError & ) ) );
#endif

	QObject::connect( this, SIGNAL( resultReceived( ) ),
		this, SLOT( handleResult( ) ) );
}

void WolframeClient::timeoutOccurred( )
{
	m_timeoutTimer->stop( );

	if( m_socket->state( ) != QAbstractSocket::ConnectedState ) {
		emit error( QAbstractSocket::SocketTimeoutError );
		m_hasErrors = true;
	}
}

#ifdef WITH_SSL
void WolframeClient::initializeSsl( )
{
	if( m_initializedSsl ) return;

// CA certificate to verify the client certificate
	if( m_connParams.SSLverify ) {
		QList<QSslCertificate> caCerts;
		QSslCertificate caCert = getCertificate( m_connParams.SSLCAbundle );
		caCerts.append( caCert );
		reinterpret_cast<QSslSocket *>( m_socket )->addCaCertificates( caCerts );
	}
	
	if( m_connParams.clientCertificate ) {
// our local client certificate we present to the server
		reinterpret_cast<QSslSocket *>( m_socket )->setLocalCertificate(
			getCertificate( m_connParams.SSLcertificate ) );
// the key for using the client certificate
		reinterpret_cast<QSslSocket *>( m_socket )->setPrivateKey( m_connParams.SSLkey );
	}
	
	m_initializedSsl = true;
}

QSslCertificate WolframeClient::getCertificate( QString filename )
{
	QFile file( filename );

	if( !file.exists( ) )
		emit error( tr( "certificate %1 doesn't exist" ).arg( filename ) );

	if( !file.open( QIODevice::ReadOnly ) )
		emit error( tr( "can't open certificate %1" ).arg( filename ) );

	QSslCertificate cert( file.readAll( ) );

	if( cert.isNull( ) )
		emit error( tr( "empty certificate in file %1" ).arg( filename ) );

	if( !cert.isValid( ) )
		emit error( tr( "certificate in %1 is invalid" ).arg( filename ) );

	return cert;
}

void WolframeClient::sslErrors( const QList<QSslError> &errors )
{
// for all other errors warn user about it
	foreach( const QSslError &e, errors ) {
		if( e.error( ) == QSslError::SelfSignedCertificateInChain) continue;
		if( e.error( ) == QSslError::HostNameMismatch) continue;
		m_hasErrors = true;	
		qDebug( ) << "SSL ERROR: " << e;
		emit error( e.errorString( ) );
	}

// ignore them		
	reinterpret_cast<QSslSocket *>( m_socket )->ignoreSslErrors( );
}

void WolframeClient::peerVerifyError( const QSslError &e )
{	
	if( e.error( ) == QSslError::SelfSignedCertificateInChain ) return;
	if( e.error( ) == QSslError::HostNameMismatch) return;
	m_hasErrors = true;
	qDebug( ) << "PEER VERIFY SSL ERROR: " << e;
	emit error( e.errorString( ) );
}

void WolframeClient::encrypted( )
{
	if( m_hasErrors ) {
		emit error( tr( "Channel is encrypted, but there were errors on the way." ) );
	}
}
#endif

WolframeClient::~WolframeClient( )
{
	delete m_socket;
}

void WolframeClient::connect( )
{
	switch( m_state ) {
		case Disconnected:
			if( m_connParams.SSL ) {
#ifdef WITH_SSL
				initializeSsl( );
				reinterpret_cast<QSslSocket *>( m_socket )->connectToHostEncrypted( m_connParams.host, m_connParams.port );
#else
				m_socket->connectToHost( m_connParams.host, m_connParams.port );
#endif
			} else {
				m_socket->connectToHost( m_connParams.host, m_connParams.port );
			}
			
			if( m_connParams.timeout > 0 ) {
				m_timeoutTimer->start( m_connParams.timeout * 1000 );
			}
			
			m_state = AboutToConnect;
			break;

		case AboutToConnect:
			emit error( tr( "Already connecting.. wait till you connect again" ) );
			break;

		case Connected:
			emit error( tr( "Disconnect first before connecting again!" ) );
			break;

		case AboutToDisconnect:
			emit error( tr( "Currently disconnected.. wait till you connect again" ) );
			break;
			
		case Data:
		default:
			emit error( tr( "ILLEGAL STATE %1 in connect!" ).arg( m_state ) );
	}
}

void WolframeClient::disconnect( )
{
	m_hasErrors = false;

	switch( m_state ) {
		case Disconnected:
			// can happen, is ok, remain in disconneted state and reemit signal
			// in case the client relies on the signal
			emit disconnected( );
			break;

		case AboutToConnect:
			emit error( tr( "Got disconnected Qt signal when about to build up a connection?!" ) );
			break;

		case AboutToDisconnect:
			m_state = Disconnected;
			break;

		case Data:
		case Connected:
			sendCommand( "QUIT" );
			m_state = AboutToDisconnect;
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in disconnect!" ).arg( m_state ) );
	}
}

void WolframeClient::error( QAbstractSocket::SocketError _error )
{
	switch( m_state ) {
		case Disconnected:
		case AboutToDisconnect:
// connection closed by server as a reaction to QUIT command (should better be "client
// goes first disconnection pattern" IMHO)
			if( _error == QAbstractSocket::RemoteHostClosedError ) {
				m_socket->close( );
				m_state = Disconnected;
				emit error( tr( "Connection closed by server." ) );
			} else {
				if( !m_hasErrors ) {
					emit error( m_socket->errorString( ) );
				}
			}
			break;

		case AboutToConnect:
// error during connection, usually server is not there, go back to disconnected state
			m_socket->close( );
			m_state = Disconnected;
			emit error( tr( "Timeout when connecting. Is the server up and running? (internal error: %1)" )
				.arg( m_socket->errorString( ) ) );
			break;

		case Connected:
		case Data:
// connection closed by server as a reaction to QUIT command (should better be "client
// goes first disconnection pattern" IMHO), can also happen, if the server closes the
// connection (e.g. shutdown)
			if( _error == QAbstractSocket::RemoteHostClosedError ) {
				m_socket->close( );
				m_state = Disconnected;
				emit error( tr( "Connection closed by server." ) );
			} else {
				emit error( m_socket->errorString( ) );
			}
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in error!" ).arg( m_state ) );
	}
}

void WolframeClient::privateConnected( )
{
	switch( m_state ) {
		case Disconnected:
// this is also funny, but again a race, switch directly to Connected state
			m_state = Connected;
			break;

		case AboutToConnect:
// yep, a connect signal was received
			m_state = Connected;
			break;

		case Connected:
			emit error( tr( "Got connected Qt signal when already in connected state!" ) );
			break;

		case AboutToDisconnect:
			emit error( tr( "Got connected Qt signal when already disconnecting!" ) );
			break;

		case Data:
		default:
			emit error( tr( "ILLEGAL STATE %1 in connected!" ).arg( m_state ) );
	}
}

void WolframeClient::privateDisconnected( )
{
	switch( m_state ) {
		case Disconnected:
			emit error( tr( "Got a disconnected Qt signal when already in disconnected state!" ) );
			break;

// abort brutally
		case AboutToConnect:
		case Connected:
		case Data:
		case AboutToDisconnect:
			m_socket->close( );
			m_state = Disconnected;
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in disconnected!" ).arg( m_state ) );
	}
}

void WolframeClient::dataAvailable( )
{
	switch( m_state ) {
		case Disconnected:
		case AboutToConnect:
			// early answer, this is a race (we send to fast without waiting for answers),
			// this is no problem, we don't consume anything and should get called again
			break;

		case AboutToDisconnect:
		case Connected:
		case Data:
			while( m_socket->canReadLine( ) ) {
				char buf[1024];
				qint64 len = m_socket->readLine( buf, sizeof( buf ) );
				if( len < 0 ) {
					qCritical( ) << "Error when reading line!!";
					break;
				}
				
				bool lineSeen = true;
				if( len > 1 ) {
					if( buf[len-1] == '\n' ) {
						buf[len-1] = '\0';
						lineSeen = true;
					}
				}
				if( len > 2 ) {
					if( buf[len-2] == '\r' ) {
						buf[len-2] = '\0';
						lineSeen = true;
					}
				}
// short buffer, no complete line read
				if( !lineSeen ) {
					m_answer.append( buf );
					continue;
				}
// protocol answer
				if( strncmp( buf, "BYE", 3 ) == 0 ) {
// BYE has no additional data, one liner
					emit resultReceived( );
// ERR has a message, one liner
				} else if( strncmp( buf, "ERR", 3 ) == 0 ) {
					m_hasErrors = true;
					m_state = Connected;
					emit error( tr( "Protocol error in command %1, received: %2." ).arg( m_command ).arg( buf + 3 ) );
// OK has a message, one liner
				} else if( strncmp( buf, "OK", 2 ) == 0 ) {
					if( len > 3 ) {
						QString paramLine = QString( QByteArray( buf+3, len-3 ) );
						m_params = paramLine.split( " " );
					}
					emit resultReceived( );
				} else if( strncmp( buf, "MECHS", 5 ) == 0 ) {
					if( len > 6 ) {
						QString paramLine = QString( QByteArray( buf+6, len-6 ) );
						m_params = paramLine.split( " " );
					}
					emit resultReceived( );
				} else if( strncmp( buf, "ANSWER", 6 ) == 0 ) {
					//emit resultReceived( );
					m_answer = "";
// end of message marker
				} else if( len > 0 && buf[0] == '.' && buf[1] == '\0' ) {
					// do not read that
// escaping of a single dot on a line
				} else if( len > 1 && buf[0] == '.' && buf[1] == '.' && buf[2] == '\0' ) {
					// escaped dot
					m_answer.append( "." );
				} else {
					m_answer.append( buf );
				}
// generic clients (aka debug window)				
				emit lineReceived( QString( QByteArray( buf, len ) ) );
			}
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in dataAvailable!" ).arg( m_state ) );
	}
}

void WolframeClient::sendLine( QString line )
{
	qint64 res;
	
	switch( m_state ) {
		case Connected:
		case Data:
			res = m_socket->write( line.toAscii( ).append( "\n" ) );
			if( res < 0 ) {
				qCritical( ) << "Write error!!";
				break;
			} else if( res != line.toAscii( ).length( ) + 1 ) {
				qCritical( ) << "Partial write!" << res << line.toAscii( ).length( );
				break;
			}		
			m_socket->flush( );
			emit lineSent( line );
			break;
		
		case AboutToConnect:
		case AboutToDisconnect:
		case Disconnected:
		default:
			emit error( tr( "ILLEGAL STATE %1 in dataAvailable!" ).arg( m_state ) );
	}
}

// high-level

void WolframeClient::sendCommand( QString command, QStringList params, QString content )
{	
	switch( m_state ) {
		case Connected:
			{
			m_state = Data;
			m_answer = "";
			m_command = command;
			m_hasErrors = false;

// append params to command, send command line	
			QString line;
			line.append( command );
			foreach( QString param, params ) {  
				line.append( ' ' );
				line.append( param );
			}
			sendLine( line );	

// send content, and terminate it
// TODO: how to distinguuish between no content (AUTH) and empty content
// (REQUEST)?
			if( !content.isEmpty( ) ) {
				QStringList lines = content.split( "\n" );
				foreach( line, lines ) {
					sendLine( line );
				}
				sendLine( "." );
			}
			}
			break;

		case AboutToConnect:
		case Data:
			{
				struct WolframeRequest r( command, params, content );
				m_queue.enqueue( r );
			}
			break;
			
		case AboutToDisconnect:
		case Disconnected:
		default:
			emit error( tr( "ILLEGAL STATE %1 in dataAvailable!" ).arg( m_state ) );
	}
}

void WolframeClient::sendCommand( struct WolframeRequest r )
{
	sendCommand( r.command, r.params, r.content );
}

void WolframeClient::sendCommand( QString command, QStringList params )
{
	sendCommand( command, params, QString( ) );
}

void WolframeClient::sendCommand( QString command )
{
	sendCommand( command, QStringList( ), QString( ) );
}

void WolframeClient::capa( )
{
	sendCommand( "CAPA" );
}

void WolframeClient::auth( )
{
	sendCommand( "AUTH" );
}

void WolframeClient::mech( QString _mech )
{
	QStringList params;
	params << _mech;
	sendCommand( "MECH", params );
}

void WolframeClient::request( QString type, QString content )
{
	QStringList params;
	params << type;
	sendCommand( "REQUEST", params, content );
}

void WolframeClient::handleResult( )
{
	m_state = Connected;
	
	qDebug( ) << "handle result of command" << m_command;
	//<< "\nparams:" << m_params << "\n:answer:" << m_answer;
	if( m_command == "CONNECT" ) {
		// swallow greeting line from server after connect
		emit connected( );
	} else if( m_command == "QUIT" ) {
		// forget outstanding commands after QUIT!
		m_queue.clear( );
		emit disconnected( );
	} else if( m_command == "AUTH" ) {
		emit mechsReceived( m_params );
	} else if( m_command == "MECH" ) {
		if( m_params[0] == "authorization" ) {
			emit authOk( );
		} else {
			emit authFailed( );
		}
	} else if( m_command == "CAPA" ) {
		emit capasReceived( m_params );
	} else if( m_command == "REQUEST" ) {
		emit answerReceived( m_params, m_answer );
	}

// still commands in the queue to execute? then do so..
	if( !m_queue.isEmpty( ) ) {
		WolframeRequest r = m_queue.dequeue( );
		sendCommand( r );
	}
}

bool WolframeClient::isConnected( ) const
{
	return( m_state == Connected || m_state == Data );
}

bool WolframeClient::isEncrypted( ) const
{
	// TODO: needs improvement! Are verifications and certs ok?
	return( isConnected( ) && m_connParams.SSL );
}
