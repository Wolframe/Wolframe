//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>
#include <QtUiTools>
#include <QCommandLine>	

#include "FormLoader.hpp"
#include "DebugTerminal.hpp"
#include "WolframeClient.hpp"
#include "FormWidget.hpp"
#include "LoginDialog.hpp"
#include "LoadMode.hpp"

#define SESSION_NAME "qtclient-db"

namespace _Wolframe {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
		virtual ~MainWindow( );		
		enum ScreenOrientation {
			ScreenOrientationLockPortrait,
			ScreenOrientationLockLandscape,
			ScreenOrientationAuto
		};

	// Note that this will only have an effect on Fremantle.
	void setOrientation( ScreenOrientation orientation );
	void showExpanded( );
	
	private:
		QCommandLine *m_cmdline;	// command line parser	
		QWidget *m_ui;			// main window from theme
		FormWidget *m_formWidget;	// current active form
		QString m_currentTheme;		// the name of the currently selected theme
		QString m_currentForm;		// the name of the form currently visible
		FormLoader *m_formLoader;	// form loader (visible form)
		DataLoader *m_dataLoader;	// load and saves data (data form)
		DebugTerminal *m_debugTerminal;	// protocol debug terminal (interactive)
		WolframeClient *m_wolframeClient; // the client protocol class
		QString m_currentLanguage;	// code of the current interface language
		QUiLoader *m_uiLoader;		// the designer UI loader
		QString m_host;			// wolframe server port to use
		unsigned short m_port;		// wolframe port to use
		bool m_secure;			// use SSL for wolframe protocol
		QString m_clientCertFile;	// filename of the client certfificate
		QString m_clientKeyFile;	// filename of the client key file
		QString m_CACertFile;		// filename of the CA certificate
		LoadMode m_loadMode;		// whether to use local loader or not
		bool m_debug;			// show debug windows from the beginning
		LoginDialog *m_loginDialog;	// the login dialog
		QString m_dbName;		// name of the local database file for storing forms
		QString m_settings;		// file to read settings from
		QString m_uiFormsDir;		// for FileFormLoader (forms dir)
		QString m_uiFormTranslationsDir; // for FileFormLoader (i18n dir)
		QString m_dataLoaderDir;	// for FileDataLoader (data dir)

	public slots:
		void readSettings( );
		void parseArgs( );
		void initialize( );
		void populateThemesMenu( );
		void loadTheme( QString themeName );
		void loadLanguages( );
		void loadForm( QString formName );
		void loadLanguage( QString language );

	private slots:
// slots for command line parsing
		void switchFound( const QString &name );
		void optionFound( const QString &name, const QVariant &value );
		void paramFound( const QString &name, const QVariant &value );
		void parseError( const QString &error );

// slots for the wolframe client		
		void wolframeError( QString error );
		void connected( );
		void disconnected( );
		void authenticationOk( );
		void authenticationFailed( );

// menu slots		
		void themeSelected( QAction *action );
		void formSelected( QAction *action );
		void languageSelected( QAction *action );

// form loader slots
		void languageCodesLoaded( QStringList languages );
		void formListLoaded( QStringList forms );
		void formLoaded( QString name );

// auto-wired slots for the menu
		void on_actionRestart_triggered( );
		void on_actionExit_triggered( );
		void on_actionPreferences_triggered( );
		void on_actionManageStorage_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
		void on_actionDebugTerminal_triggered( bool checked );       
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _MAIN_WINDOW_HPP_INCLUDED
