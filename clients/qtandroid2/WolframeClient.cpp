//
// WolframeClient.cpp
//

#include "WolframeClient.hpp"

#include <QByteArray>
#include <QTcpSocket>
#include <QFile>
#include <QList>

namespace _Wolframe {
	namespace QtClient {

WolframeClient::WolframeClient( QString _host, unsigned short _port, bool _secure, QString _clientCertFile, QString _clientKeyFile, QString _CACertfile, QWidget *_parent ) :
	m_host( _host ),
	m_port( _port ),
	m_secure( _secure ),
	m_clientCertFile( _clientCertFile ),
	m_clientKeyFile( _clientKeyFile ),
	m_CACertFile( _CACertfile ),	
	m_state( Disconnected ),
	m_timeout( 4000 ),
	m_parent( _parent ),
	m_hasErrors( false ),
#ifdef WITH_SSL
	m_initializedSsl( false ),
#endif
	m_command( "CONNECT" )
{
#ifdef WITH_SSL
	m_socket = new QSslSocket( this );
#else
	m_socket = new QTcpSocket( this );
#endif

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

    qDebug( ) << "wolframeclient created";
}

#ifdef WITH_SSL
void WolframeClient::initializeSsl( )
{
	if( m_initializedSsl ) return;

	QList<QSslCertificate> certs;
// CA certificate to verify the client certificate
	certs.append( getCertificate( m_CACertFile ) );
	reinterpret_cast<QSslSocket *>( m_socket )->addCaCertificates( certs );
// our local client certificate we present to the server
	reinterpret_cast<QSslSocket *>( m_socket )->setLocalCertificate(
		getCertificate( m_clientCertFile ) );
// the key for using the client certificate
	reinterpret_cast<QSslSocket *>( m_socket )->setPrivateKey( m_clientKeyFile );

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
	m_hasErrors = true;
	foreach( const QSslError &e, errors )
		emit error( e.errorString( ) );

	// ignore for now
	reinterpret_cast<QSslSocket *>( m_socket )->ignoreSslErrors( );
}

void WolframeClient::peerVerifyError( const QSslError &e )
{
	m_hasErrors = true;
	emit error( e.errorString( ) );
}

void WolframeClient::encrypted( )
{
	emit error( m_hasErrors ? tr( "Channel is encrypted, but there were errors on the way." ) : tr( "Channel is encrypted now." ) );
}
#endif

WolframeClient::~WolframeClient( )
{
	delete m_socket;
}

void WolframeClient::connect( )
{
    qDebug( ) << "wolframeclient connect";
	switch( m_state ) {
		case Disconnected:
			if( m_secure ) {
#ifdef WITH_SSL
				initializeSsl( );
				reinterpret_cast<QSslSocket *>( m_socket )->connectToHostEncrypted( m_host, m_port );
#else
				m_socket->connectToHost( m_host, m_port );
#endif
			} else {
				m_socket->connectToHost( m_host, m_port );
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

		default:
			emit error( tr( "ILLEGAL STATE %1 in connect!" ).arg( m_state ) );
	}
}

void WolframeClient::disconnect( )
{
    qDebug( ) << "wolframeclient disconnect";

    m_hasErrors = false;

	switch( m_state ) {
		case Disconnected:
			emit error( tr( "Got disconnected Qt signal when already in disconnected state!" ) );
			break;

		case AboutToConnect:
			emit error( tr( "Got disconnected Qt signal when about to build up a connection?!" ) );
			break;

		case AboutToDisconnect:
			m_state = Disconnected;
			break;

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
				emit error( m_socket->errorString( ) );
			}
			break;

		case AboutToConnect:
// error during connection, usually server is not there, go back to disconnected state
			m_socket->close( );
			m_state = Disconnected;
			emit error( m_socket->errorString( ) );
			break;

		case Connected:
// connection closed by server as a reaction to QUIT command (should better be "client
// goes first disconnection pattern" IMHO)
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
			emit error( tr( "Got a connect Qt signal when not starting a new connection!" ) );
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

		default:
			emit error( tr( "ILLEGAL STATE %1 in connected!" ).arg( m_state ) );
	}
}

void WolframeClient::privateDisconnected( )
{
	switch( m_state ) {
		case AboutToConnect:
		case Disconnected:
		case Connected:
			emit error( tr( "Got a disconnected Qt signal when already in disconnected state!" ) );
			break;

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
			emit error( tr( "Invalid state, got data while in state %1?" ).arg( m_state ) );
			break;

		case AboutToDisconnect:
		case Connected:
			while( m_socket->canReadLine( ) ) {
				char buf[1024];
				qint64 len = m_socket->readLine( buf, sizeof( buf ) );
				if( len > 1 )
					if( buf[len-1] == '\n' ) buf[len-1] = '\0';
				if( len > 2 )
					if( buf[len-2] == '\r' ) buf[len-2] = '\0';
// protocol answer
				if( strncmp( buf, "BYE", 3 ) == 0 ) {
// BYE has no additional data, one liner
					emit resultReceived( );
// ERR has a message, one liner
				} else if( strncmp( buf, "ERR", 3 ) == 0 ) {
					m_hasErrors = true;
					emit error( tr( "Protocol error, received: %1." ).arg( buf + 3 ) );
// OK has a message, one liner
				} else if( strncmp( buf, "OK", 2 ) == 0 ) {
					if( len > 3 ) {
						m_answer = QString( QByteArray( buf+3, len-3 ) );
					}
					emit resultReceived( );
				} else if( strncmp( buf, "MECHS", 5 ) == 0 ) {
					if( len > 6 ) {
						m_answer = QString( QByteArray( buf+6, len-6 ) );
					}
					emit resultReceived( );
				} else if( buf[0] == '.' && buf[1] == '\n' ) {
					emit resultReceived( );
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
	m_socket->write( line.toAscii( ).append( "\n" ) );
	m_socket->flush( );
	emit lineSent( line );
}

// high-level
void WolframeClient::sendCommand( QString command )
{
	m_answer = "";
	m_command = command;
	m_hasErrors = false;
	sendLine( command );
}

void WolframeClient::sendCommand( QString command, QStringList params )
{
	QString line;
	
	m_answer = "";
	m_command = command;
	m_hasErrors = false;
	
	line.append( command );
	foreach( QString param, params ) {  
		line.append( ' ' );
		line.append( param );
	}
	
	sendLine( line );	
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

void WolframeClient::handleResult( )
{
	qDebug( ) << "handle result" << m_command << m_answer;
	if( m_command == "CONNECT" ) {
		// swallow greeting line from server after connect
		emit connected( );
	} else if( m_command == "QUIT" ) {
		emit disconnected( );
	} else if( m_command == "AUTH" ) {
		QStringList mechList = m_answer.split( " \t", QString::SkipEmptyParts );
		emit mechsReceived( mechList );
	} else if( m_command == "MECH" ) {
		if( m_answer == "authorization" ) {
			emit authOk( );
		} else {
			emit authFailed( );
		}
	}
}

} // namespace QtClient
} // namespace _Wolframe
