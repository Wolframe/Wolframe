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
		
		Q_PROPERTY( LoadMode uiLoadMode READ uiLoadMode WRITE setUiLoadMode )
		LoadMode uiLoadMode( ) const { return m_uiLoadMode; }
		void setUiLoadMode( LoadMode _uiLoadMode ) { m_uiLoadMode = _uiLoadMode; }

		Q_PROPERTY( LoadMode dataLoadMode READ dataLoadMode WRITE setDataLoadMode )
		LoadMode dataLoadMode( ) const { return m_dataLoadMode; }
		void setDataLoadMode( LoadMode _dataLoadMode ) { m_dataLoadMode = _dataLoadMode; }
					
		Q_PROPERTY( bool debug READ debug WRITE setDebug )
		bool debug( ) const { return m_debug; }
		void setDebug( bool _debug ) { m_debug = _debug; }

		Q_PROPERTY( bool developer READ developer WRITE setDeveloper )
		bool developer( ) const { return m_developer; }
		void setDeveloper( bool _developer ) { m_developer = _developer; }

		Q_PROPERTY( QString uiFormsDir READ uiFormsDir WRITE setUiFormsDir )
		QString uiFormsDir( ) const { return m_uiFormsDir; }
		void setUiFormsDir( QString _uiFormsDir ) { m_uiFormsDir = _uiFormsDir; }

		Q_PROPERTY( QString uiFormTranslationsDir READ uiFormTranslationsDir WRITE setUiFormTranslationsDir )
		QString uiFormTranslationsDir( ) const { return m_uiFormTranslationsDir; }
		void setUiFormTranslationsDir( QString _uiFormTranslationsDir ) { m_uiFormTranslationsDir = _uiFormTranslationsDir; }
		
		Q_PROPERTY( QString uiFormResourcesDir READ uiFormResourcesDir WRITE setUiFormResourcesDir )
		QString uiFormResourcesDir( ) const { return m_uiFormResourcesDir; }
		void setUiFormResourcesDir( QString _uiFormResourcesDir ) { m_uiFormResourcesDir = _uiFormResourcesDir; }
		
		Q_PROPERTY( QString dataLoaderDir READ dataLoaderDir WRITE setDataLoaderDir )
		QString dataLoaderDir( ) const { return m_dataLoaderDir; }
		void setDataLoaderDir( QString _dataLoaderDir ) { m_dataLoaderDir = _dataLoaderDir; }
	
		Q_PROPERTY( QString locale READ locale WRITE setLocale )
		QString locale( ) const { return m_locale; }
		void setLocale( QString _locale ) { m_locale = _locale; }

		Q_PROPERTY( bool mdi READ mdi WRITE setMdi )
		bool mdi( ) const { return m_mdi; }
		void setMdi( bool _mdi ) { m_mdi = _mdi; }
		
	private:
		QSettings *createSettings( );
		
	private:
		static QScopedPointer<Preferences> m_instance;
		static QString m_fileName;
		
		QString m_organization;
		QString m_application;
		
		LoadMode m_uiLoadMode;
		LoadMode m_dataLoadMode;
		bool m_debug;
		bool m_developer;
		QString m_uiFormsDir;
		QString m_uiFormTranslationsDir;
		QString m_uiFormResourcesDir;
		QString m_dataLoaderDir;
		QString m_locale;
		bool m_mdi;
};	

#endif // _PREFERENCES_HPP_INCLUDED
