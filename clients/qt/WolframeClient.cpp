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

#include "WolframeClient.hpp"

#include <QByteArray>
#include <QTcpSocket>
#include <QFile>
#include <QList>
#include <QTimer>
#ifdef WITH_SSL
#include <QSslCipher>
#endif

WolframeClient::WolframeClient( const ConnectionParameters _connParams,	QWidget *_parent )
	: m_connParams( _connParams )
	,m_state( Disconnected )
	,m_hasErrors( false )
#ifdef WITH_SSL
	,m_initializedSsl( false )
#endif
{
#ifdef WITH_SSL
	m_socket = new QSslSocket( this );
#else
	m_socket = new QTcpSocket( this );
#endif
	m_protocol.initSocket( m_socket);
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

	QObject::connect( this, SIGNAL( resultHandled( ) ),
		this, SLOT( dataAvailable( ) ) );
	QObject::connect( this, SIGNAL( resultReceived( ) ),
		this, SLOT( handleResult( ) ) );
}

void WolframeClient::setConnectionParameters( const ConnectionParameters _connParams )
{
	m_connParams = _connParams;
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
		qobject_cast<QSslSocket *>( m_socket )->addCaCertificates( caCerts );
	}

	if( m_connParams.clientCertificate ) {
// our local client certificate we present to the server
		qobject_cast<QSslSocket *>( m_socket )->setLocalCertificate(
			getCertificate( m_connParams.SSLcertificate ) );
// the key for using the client certificate
		qobject_cast<QSslSocket *>( m_socket )->setPrivateKey( m_connParams.SSLkey );
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
	qobject_cast<QSslSocket *>( m_socket )->ignoreSslErrors( );
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
				qobject_cast<QSslSocket *>( m_socket )->connectToHostEncrypted( m_connParams.host, m_connParams.port );
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
			m_protocol.quit();
			m_protocol.process();
			m_state = AboutToDisconnect;
			break;

		case Connected:
			m_protocol.quit();
			m_protocol.process();
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
//				emit error( tr( "Connection closed by server." ) );
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
				//emit error( tr( "Connection closed by server." ) );
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
	bool isAuth;
	bool success;
	switch( m_state ) {
		case Disconnected:
		case AboutToConnect:
			// early answer, this is a race (we send to fast without waiting for answers),
			// this is no problem, we don't consume anything and should get called again
			break;

		case AboutToDisconnect:
		case Connected:
		case Data:
			isAuth = m_protocol.isAuthorized();
			success = m_protocol.process();
			if (!isAuth && m_protocol.isAuthorized())
			{
				emit authOk();
			}
			if (!success)
			{
				if (m_protocol.getLastError())
				{
					qCritical() << *m_protocol.getLastError();
					emit error( tr( "error in protocol: %1").arg( *m_protocol.getLastError()));
				}
				break;
			}
			if (m_protocol.getAnswerTag())
			{
				emit resultReceived();
			}
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in dataAvailable!" ).arg( m_state ) );
	}
}

// high-level
void WolframeClient::request( const QString& tag, const QByteArray& content )
{
	m_protocol.pushRequest( tag, content);
}

void WolframeClient::handleResult( )
{
	m_state = Connected;

	const QString* tag;
	while ((tag=m_protocol.getAnswerTag()) != 0)
	{
		bool success = m_protocol.getAnswerSuccess();
		const QByteArray* content = m_protocol.getAnswerContent();

		qDebug( ) << "handle result of command" << *tag;
		emit answerReceived( success, *tag, *content);
		m_protocol.removeAnswer();
		emit resultHandled();
	}
}

void WolframeClient::auth()
{
	m_protocol.authorize();
	m_protocol.process();
}

bool WolframeClient::isConnected( ) const
{
	return( m_state == Connected || m_state == Data );
}

const QString WolframeClient::serverName() const
{
	if ( m_state == Connected || m_state == Data )
		return m_connParams.name;
	else
		return "";
}

bool WolframeClient::isEncrypted( ) const
{
	// TODO: needs improvement! Are verifications and certs ok?
	return( isConnected( ) && m_connParams.SSL );
}

const QString WolframeClient::encryptionName() const
{
	if ( isConnected( ) && m_connParams.SSL )
#ifdef WITH_SSL
		return qobject_cast<QSslSocket *>( m_socket )->sessionCipher().name();
#else
		return "";
#endif
	else
		return "";
}
