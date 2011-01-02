//
// SMERPClient.cpp
//

#include "SMERPClient.hpp"

#include <QMessageBox>

namespace _SMERP {
	namespace QtClient {

SMERPClient::SMERPClient( QWidget *_parent ) :
	m_state( Disconnected ),
	m_parent( _parent )
{
	m_socket = new QTcpSocket( this );

	QObject::connect( m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ),
		this, SLOT( error( QAbstractSocket::SocketError ) ) );
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
			break;

		case Connected:
			m_socket->close( );
			m_socket->connectToHost( m_host, m_port );
			break;
	}
}

void SMERPClient::disconnect( )
{
	switch( m_state ) {
		case Disconnected:
			break;

		case Connected:
			m_socket->close( );
			break;	}
}

void SMERPClient::error( QAbstractSocket::SocketError _error )
{
	emit error( m_socket->errorString( ) );
}

} // namespace QtClient
} // namespace _SMERP
