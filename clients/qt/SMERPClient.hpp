//
// SMERPClient.hpp
//

#ifndef _Wolframe_CLIENT_HPP_INCLUDED
#define _Wolframe_CLIENT_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QAbstractSocket>
#ifdef WITH_SSL
#include <QSslSocket>
#include <QSslCertificate>
#include <QList>
#endif

namespace _Wolframe {
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
		bool m_hasErrors;
#ifdef WITH_SSL
		bool m_initializedSsl;
#endif

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

#ifdef WITH_SSL		
		Q_PROPERTY( bool m_secure READ secure WRITE setSecure )
		bool secure( ) const { return m_secure; }
		void setSecure( bool _secure ) { m_secure = _secure; }
#endif

	private slots:
		void error( QAbstractSocket::SocketError );
#ifdef WITH_SSL
		void initializeSsl( );
		QSslCertificate getCertificate( QString filename );
		void sslErrors( const QList<QSslError> &errors );
		void encrypted( );
		void peerVerifyError( const QSslError &e );
#endif
		void dataAvailable( );
		void connected( );
		void disconnected( );

	signals:
		void error( QString error );
		void lineReceived( QString line );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _Wolframe_CLIENT_HPP_INCLUDED
