//
// SMERPClient.hpp
//

#ifndef _SMERP_CLIENT_HPP_INCLUDED
#define _SMERP_CLIENT_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QtNetwork/QAbstractSocket>

namespace _SMERP {
	namespace QtClient {

	class SMERPClient : public QObject
	{
	Q_OBJECT

	public:
		enum State {
			Disconnected,
			AboutToConnect,
			Connected,
			AboutToDisconnect
		};
	

	private:
		QString m_host;
		unsigned short m_port;
		bool m_secure;
		State m_state;
		QAbstractSocket *m_socket;
		QWidget *m_parent;

	public:
		SMERPClient( QWidget *_parent = 0 );
		virtual ~SMERPClient( );

		void connect( );
		void disconnect( );
		void sendLine( QString line );

		Q_PROPERTY( QString m_host READ host WRITE setHost )
		QString host( ) const { return m_host; }
		void setHost( QString _host ) { m_host = _host; }

		Q_PROPERTY( unsigned short m_port READ port WRITE setPort )
		unsigned short port( ) const { return m_port; }
		void setPort( unsigned short _port ) { m_port = _port; }
		
		Q_PROPERTY( bool m_secure READ secure WRITE setSecure )
		bool secure( ) const { return m_secure; }
		void setSecure( bool _secure ) { m_secure = _secure; }

	private slots:
		void error( QAbstractSocket::SocketError );
		void dataAvailable( );
		void connected( );
		void disconnected( );

	signals:
		void error( QString error );
		void lineReceived( QString line );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _SMERP_CLIENT_HPP_INCLUDED
