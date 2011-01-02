//
// SMERPClient.hpp
//

#ifndef _SMERP_CLIENT_HPP_INCLUDED
#define _SMERP_CLIENT_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QtNetwork>

namespace _SMERP {
	namespace QtClient {

	class SMERPClient : public QObject
	{
	Q_OBJECT

	public:
		enum State {
			Disconnected,
			Connected
		};
	

	private:
		QString m_host;
		unsigned short m_port;
		State m_state;
		QTcpSocket *m_socket;
		QWidget *m_parent;

	public:
		SMERPClient( QWidget *_parent = 0 );
		virtual ~SMERPClient( );

		void connect( );
		void disconnect( );

		Q_PROPERTY( QString m_host READ host WRITE setHost )
		QString host( ) const { return m_host; }
		void setHost( QString _host ) { m_host = _host; }

		Q_PROPERTY( unsigned short m_port READ port WRITE setPort )
		unsigned short port( ) const { return m_port; }
		void setPort( unsigned short _port ) { m_port = _port; }

	private slots:
		void error( QAbstractSocket::SocketError );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _SMERP_CLIENT_HPP_INCLUDED
