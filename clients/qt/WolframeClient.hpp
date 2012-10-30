//
// WolframeClient.hpp
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

	class WolframeClient : public QObject
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
		QString m_clientCertFile;	// filename of the client certfificate
		QString m_clientKeyFile;	// filename of the client key file
		QString m_CACertFile;		// filename of the CA certificate
		State m_state;
		unsigned int m_timeout;
		QAbstractSocket *m_socket;
		QWidget *m_parent;
		bool m_hasErrors;
#ifdef WITH_SSL
		bool m_initializedSsl;
#endif
		QString m_answer;
		QString m_command;

	public:
		WolframeClient( QString host = "localhost", unsigned short port = 7661,
			bool secure = false, QString _clientCertFile = "./certs/client.crt",
			QString _clientKeyFile = "./private/client.key",
			QString _CACertfile = "./certs/CAclient.cert.crt",
			QWidget *_parent = 0 );
		virtual ~WolframeClient( );

// low-level commands, pre-protocol, for debugging mainly
		void connect( );
		void disconnect( );
		void sendLine( QString line );

// generic send command function, implementing the frame of the protocol
		void sendCommand( QString command );
		void sendCommand( QString command, QStringList params );

// high-level commands
		void auth( );
		void mech( QString mech );
		void login( QString username, QString password );
		void run( QString cmd, QString data = QString::null );

// synchonous commands
		bool syncConnect( );
		QStringList syncAuth( );
		bool sync( QString mech );
		bool syncLogin( QString username, QString password );
		QString syncRun( QString cmd, QString data = QString::null );

		Q_PROPERTY( QString m_host READ host WRITE setHost )
		QString host( ) const { return m_host; }
		void setHost( QString _host ) { m_host = _host; }

		Q_PROPERTY( unsigned short m_port READ port WRITE setPort )
		unsigned short port( ) const { return m_port; }
		void setPort( unsigned short _port ) { m_port = _port; }

		Q_PROPERTY( unsigned int m_timeout READ timeout WRITE setTimeout )
		unsigned int timeout( ) const { return m_timeout; }
		void setTimeout( unsigned int _timeout ) { m_timeout = _timeout; }

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
		void privateConnected( );
		void disconnected( );

		void handleResult( );

	signals:
		void connected( );
		void error( QString error );

// low-level commands, pre-protocol, for debugging mainly
		void lineReceived( QString line );
		void lineSent( QString line );

// generic implementation of a command execute implementing the frame of the protocol
		void resultReceived( );
		void mechsReceived( QStringList mechList );
		void authOk( );
		void authFailed( );
		void loginOk( );
		void loginFailed( );
		void runReceived( QString cmd, QString answer );

// high-level commands
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _Wolframe_CLIENT_HPP_INCLUDED
