//
// SMERPClient.cpp
//

#include "SMERPClient.hpp"

#include <QByteArray>
#include <QTcpSocket>
#include <QFile>

namespace _SMERP {
	namespace QtClient {

SMERPClient::SMERPClient( QWidget *_parent ) :
	m_state( Disconnected ),
	m_parent( _parent ),
	m_hasErrors( false )
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
		this, SLOT( connected( ) ) );
	QObject::connect( m_socket, SIGNAL( disconnected( ) ),
		this, SLOT( disconnected( ) ) );
#ifdef WITH_SSL
	QObject::connect( m_socket, SIGNAL( sslErrors( const QList<QSslError> & ) ),
		this, SLOT( sslErrors( const QList<QSslError> & ) ) );
	QObject::connect( m_socket, SIGNAL( encrypted( ) ),
		this, SLOT( encrypted( ) ) );
	QObject::connect( m_socket, SIGNAL( peerVerifyError( const QSslError & ) ),
		this, SLOT( peerVerifyError( const QSslError & ) ) );
#endif

#ifdef WITH_SSL
	reinterpret_cast<QSslSocket *>( m_socket )->setLocalCertificate( "./client.crt" );
	reinterpret_cast<QSslSocket *>( m_socket )->setPrivateKey( "./client.key" );
	QFile CAfile( "CAclient.cert.pem" );
	if( CAfile.open( QIODevice::ReadOnly | QIODevice::Text ) )
		reinterpret_cast<QSslSocket *>( m_socket )->addCaCertificate( CAfile.readAll( ) );
	QFile CAfile2( "CAclient.cert.pem" );
	if( CAfile2.open( QIODevice::ReadOnly | QIODevice::Text ) )
		reinterpret_cast<QSslSocket *>( m_socket )->addCaCertificate( CAfile2.readAll( ) );
#endif
}

#ifdef WITH_SSL
void SMERPClient::sslErrors( const QList<QSslError> &errors )
{
	m_hasErrors = true;
	foreach( const QSslError &e, errors )
		emit error( e.errorString( ) );

	// ignore for now
	reinterpret_cast<QSslSocket *>( m_socket )->ignoreSslErrors( );
}

void SMERPClient::peerVerifyError( const QSslError &e )
{
	m_hasErrors = true;
	emit error( e.errorString( ) );
}

void SMERPClient::encrypted( )
{
	emit error( m_hasErrors ? tr( "Channel is encrypted, but there were errors on the way." ) : tr( "Channel is encrypted now." ) );
}
#endif

SMERPClient::~SMERPClient( )
{
	delete m_socket;
}

void SMERPClient::connect( )
{
	switch( m_state ) {
		case Disconnected:
			if( m_secure ) {
#ifdef WITH_SSL
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

void SMERPClient::disconnect( )
{
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
			m_socket->write( QByteArray( "quit" ) );
			m_state = AboutToDisconnect;
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in disconnect!" ).arg( m_state ) );
	}
}

void SMERPClient::error( QAbstractSocket::SocketError _error )
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

void SMERPClient::connected( )
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
			emit error( tr( "Got dconnected Qt signal when already disconnecting!" ) );
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in connected!" ).arg( m_state ) );
	}
}

void SMERPClient::disconnected( )
{
	switch( m_state ) {
		case AboutToConnect:
		case Disconnected:
		case Connected:
			emit error( tr( "Got a disconnected Qt signal when already in illegal state!" ) );
			break;

		case AboutToDisconnect:
			m_socket->close( );
			m_state = Disconnected;
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in disconnected!" ).arg( m_state ) );
	}
}

void SMERPClient::dataAvailable( )
{
	switch( m_state ) {
		case Disconnected:
		case AboutToConnect:
			emit error( tr( "Invalid state, got data while in state %1?" ).arg( m_state ) );
			break;

		case AboutToDisconnect:
		case Connected:
			if( m_socket->canReadLine( ) ) {
				char buf[1024];
				qint64 len = m_socket->readLine( buf, sizeof( buf ) );
				if( buf[len-1] == '\n' ) buf[len-1] = '\0';
				emit lineReceived( QString( QByteArray( buf, len ) ) );
			}
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in dataAvailable!" ).arg( m_state ) );
	}
}

void SMERPClient::sendLine( QString line )
{
	m_socket->write( line.toAscii( ).append( "\n" ) );
	m_socket->flush( );
}

} // namespace QtClient
} // namespace _SMERP
