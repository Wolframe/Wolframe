//
// Preferences.hpp
//

#ifndef _PREFERENCES_HPP_INCLUDED
#define _PREFERENCES_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QSettings>

#include "global.hpp"

#define DEFAULT_DOMAIN "wolframe"
#define DEFAULT_APP "qtclient"


	class Preferences : public QObject
	{
		Q_OBJECT
		Q_ENUMS( LoadMode )
		
		public:
			#include "LoadMode.hpp"
			
			Preferences( QString organization = ORGANIZATION_NAME, QString application = APPLICATION_NAME, QObject *parent = 0 );
			virtual ~Preferences( );
			
			void loadSettings( );
			void storeSettings( );
			bool exists( );

			static void setFileName( const QString &fileName );			
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
			
			Q_PROPERTY( LoadMode m_uiLoadMode READ uiLoadMode WRITE setUiLoadMode )
			LoadMode uiLoadMode( ) const { return m_uiLoadMode; }
			void setUiLoadMode( LoadMode _uiLoadMode ) { m_uiLoadMode = _uiLoadMode; }

			Q_PROPERTY( LoadMode m_dataLoadMode READ dataLoadMode WRITE setDataLoadMode )
			LoadMode dataLoadMode( ) const { return m_dataLoadMode; }
			void setDataLoadMode( LoadMode _dataLoadMode ) { m_dataLoadMode = _dataLoadMode; }
			
			Q_PROPERTY( QString m_dbName READ dbName WRITE setDbName )
			QString dbName( ) const { return m_dbName; }
			void setDbName( QString _dbName ) { m_dbName = _dbName; }
			
			Q_PROPERTY( bool m_debug READ debug WRITE setDebug )
			bool debug( ) const { return m_debug; }
			void setDebug( bool _debug ) { m_debug = _debug; }

			Q_PROPERTY( QString m_uiFormsDir READ uiFormsDir WRITE setUiFormsDir )
			QString uiFormsDir( ) const { return m_uiFormsDir; }
			void setUiFormsDir( QString _uiFormsDir ) { m_uiFormsDir = _uiFormsDir; }

			Q_PROPERTY( QString m_uiFormTranslationsDir READ uiFormTranslationsDir WRITE setUiFormTranslationsDir )
			QString uiFormTranslationsDir( ) const { return m_uiFormTranslationsDir; }
			void setUiFormTranslationsDir( QString _uiFormTranslationsDir ) { m_uiFormTranslationsDir = _uiFormTranslationsDir; }
			
			Q_PROPERTY( QString m_dataLoaderDir READ dataLoaderDir WRITE setDataLoaderDir )
			QString dataLoaderDir( ) const { return m_dataLoaderDir; }
			void setDataLoaderDir( QString _dataLoaderDir ) { m_dataLoaderDir = _dataLoaderDir; }
		
			Q_PROPERTY( QString m_locale READ locale WRITE setLocale )
			QString locale( ) const { return m_locale; }
			void setLocale( QString _locale ) { m_locale = _locale; }
			
		private:
			QSettings *createSettings( );
			
		private:
			static QScopedPointer<Preferences> m_instance;
			static QString m_fileName;
			
			QString m_organization;
			QString m_application;
			
			QString m_host;
			unsigned short m_port;
			bool m_secure;
			QString m_clientCertFile;
			QString m_clientKeyFile;
			QString m_CACertFile;
			LoadMode m_uiLoadMode;
			LoadMode m_dataLoadMode;
			QString m_dbName;
			bool m_debug;
			QString m_uiFormsDir;
			QString m_uiFormTranslationsDir;
			QString m_dataLoaderDir;
			QString m_locale;
	};
	

#endif // _PREFERENCES_HPP_INCLUDED
