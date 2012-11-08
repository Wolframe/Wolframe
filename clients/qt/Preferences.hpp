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
		
		private:
			static QScopedPointer<Preferences> m_instance;
			
			QString m_host;
			unsigned short m_port;
	};
	
} // namespace QtClient
} // namespace _Wolframe

#endif // _PREFERENCES_HPP_INCLUDED