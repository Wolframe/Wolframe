//
// SMERPClient.cpp
//

#include "unused.h"
#include "SMERPClient.hpp"

#include <QMessageBox>
#include <QByteArray>

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

		case Connected:
			m_socket->write( QByteArray( "quit" ) );
			m_state = AboutToDisconnect;
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in disconnect!" ).arg( m_state ) );
	}
}

void SMERPClient::error( QAbstractSocket::SocketError _error SMERP_UNUSED )
{
	switch( m_state ) {
		case Disconnected:
		case AboutToDisconnect:
			emit error( m_socket->errorString( ) );
			break;

		case AboutToConnect:
// error during connection, usually server is not there, go back to disconnected state
			m_socket->close( );
			m_state = Disconnected;
			emit error( m_socket->errorString( ) );
			break;

		case Connected:
// TODO: depending on the error we must react here most likely, for now we are chicken
			m_socket->close( );
			m_state = Disconnected;
			emit error( m_socket->errorString( ) );
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in error!" ).arg( m_state ) );
	}
}

void SMERPClient::connected( )
{
	switch( m_state ) {
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

		case Disconnected:
			emit error( tr( "Got a connect Qt signal when not starting a new connection!" ) );
			break;

		default:
			emit error( tr( "ILLEGAL STATE %1 in connected!" ).arg( m_state ) );
	}
}

void SMERPClient::disconnected( )
{
	switch( m_state ) {
		case Disconnected:
			emit error( tr( "Got a disconnected Qt signal when already in connected state!" ) );
			break;

		case Connected:
			m_socket->close( );
			m_state = Disconnected;
			break;
	}
}

void SMERPClient::dataAvailable( )
{
	switch( m_state ) {
		case Disconnected:
			emit error( tr( "Invalid state, got data while disconnected?" ) );
			break;

		case Connected:
			if( m_socket->canReadLine( ) ) {
				char buf[1024];
				qint64 len = m_socket->readLine( buf, sizeof( buf ) );
				emit lineReceived( QString( QByteArray( buf, len ) ) );
			}
	}
}

} // namespace QtClient
} // namespace _SMERP
