//
// SMERPClient.cpp
//

#include "unused.h"
#include "SMERPClient.hpp"

#include <QByteArray>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QTcpSocket>

namespace _SMERP {
	namespace QtClient {

SMERPClient::SMERPClient( QWidget *_parent ) :
	m_state( Disconnected ),
	m_parent( _parent )
{
	m_socket = new QTcpSocket( this );

	QObject::connect( m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ),
		this, SLOT( error( QAbstractSocket::SocketError ) ) );
	QObject::connect( m_socket, SIGNAL( readyRead( ) ),
		this, SLOT( dataAvailable( ) ) );
	QObject::connect( m_socket, SIGNAL( connected( ) ),
		this, SLOT( connected( ) ) );
	QObject::disconnect( m_socket, SIGNAL( disconnected( ) ),
		this, SLOT( disconnected( ) ) );
}

SMERPClient::~SMERPClient( )
{
	delete m_socket;
}

void SMERPClient::connect( )
{
	switch( m_state ) {
		case Disconnected:
			m_socket->connectToHost( m_host, m_port );
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
