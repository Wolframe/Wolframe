//
// Preferences.hpp
//

#ifndef _PREFERENCES_HPP_INCLUDED
#define _PREFERENCES_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QScopedPointer>

#define SETTINGS_DOMAIN "wolframe.org"
#define SETTINGS_APP "qtclient"

namespace _Wolframe {
	namespace QtClient {

	class Preferences : public QObject
	{
		Q_OBJECT
		
		public:
			Preferences( QObject *parent = 0 );
			virtual ~Preferences( );
			
			void loadSettings( );
			void storeSettings( );
			
			static Preferences *instance( );

			Q_PROPERTY( QString m_host READ host WRITE setHost )
			QString host( ) const { return m_host; }
			void setHost( QString _host ) { m_host = _host; }

			Q_PROPERTY( unsigned short m_port READ port WRITE setPort )
			unsigned short port( ) const { return m_port; }
			void setPort( unsigned short _port ) { m_port = _port; }
			
			Q_PROPERTY( bool m_secure READ secure WRITE setSecure )
			bool secure( ) const { return m_secure; }
			void setSecure( bool _secure ) { m_secure = _secure; }
			
			Q_PROPERTY( QString m_clientCertFile READ clientCertFile WRITE setClientCertFile )
			QString clientCertFile( ) const { return m_clientCertFile; }
			void setClientCertFile( QString _clientCertFile ) { m_clientCertFile = _clientCertFile; }
			
			Q_PROPERTY( QString m_clientKeyFile READ clientKeyFile WRITE setClientKeyFile )
			QString clientKeyFile( ) const { return m_clientKeyFile; }
			void setClientKeyFile( QString _clientKeyFile ) { m_clientKeyFile = _clientKeyFile; }

			Q_PROPERTY( QString m_CACertFile READ caCertFile WRITE setCaCertFile )
			QString caCertFile( ) const { return m_CACertFile; }
			void setCaCertFile( QString _CACertFile ) { m_CACertFile = _CACertFile; }
			
		private:
			static QScopedPointer<Preferences> m_instance;
			
			QString m_host;
			unsigned short m_port;
			bool m_secure;
			QString m_clientCertFile;
			QString m_clientKeyFile;
			QString m_CACertFile;
	};
	
} // namespace QtClient
} // namespace _Wolframe

#endif // _PREFERENCES_HPP_INCLUDED
