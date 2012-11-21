//
// MainWindow.cpp
//

#include "MainWindow.hpp"
#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"
#include "NetworkFormLoader.hpp"
#include "NetworkDataLoader.hpp"
#include "SqliteFormLoader.hpp"
#include "SqliteDataLoader.hpp"
#include "Preferences.hpp"
#include "PreferencesDialog.hpp"
#include "ManageStorageDialog.hpp"
#include "global.hpp"

#include <QtGui>
#include <QBuffer>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QtAlgorithms>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>


// built-in defaults
MainWindow::MainWindow( QWidget *_parent ) : QWidget( _parent ),
	m_cmdline( 0 ),
	m_ui( 0 ), m_formWidget( 0 ), m_formLoader( 0 ), m_dataLoader( 0 ),
	m_debugTerminal( 0 ), m_wolframeClient( 0 ), m_uiLoader( 0 ),
	m_host( "localhost" ), m_port( 7661 ), m_secure( false ),
	m_clientCertFile( "./certs/client.crt" ), m_clientKeyFile( "./private/client.key" ),
	m_CACertFile( "./certs/CAclient.cert.pem" ),
	m_uiLoadMode( Network ), m_dataLoadMode( Network ), m_debug( false ),
	m_loginDialog( 0 ), m_dbName( DEFAULT_SQLITE_FILENAME ), m_settings( ),
	m_uiFormsDir( DEFAULT_UI_FORMS_DIR ),
	m_uiFormTranslationsDir( DEFAULT_UI_FORM_TRANSLATIONS_DIR ),
	m_dataLoaderDir( DEFAULT_DATA_LOADER_DIR ),
	m_languages( ), m_language( )
{
// read arguments for the '-s <setting file>' parameter
#ifndef Q_OS_ANDROID
	parseArgs( );
#endif
	
// settings override built-in defaults
	if( !m_settings.isNull( ) ) {
		Preferences::setFileName( m_settings );
	}
	if( !Preferences::instance( )->exists( ) ) {
		PreferencesDialog d( this );
		d.exec( );
		readSettings( );
	} else {
		readSettings( );
	}
	
// command line options override settings
#ifndef Q_OS_ANDROID
	parseArgs( );
#endif

	initialize( );
}

void MainWindow::readSettings( )
{
	Preferences *prefs = Preferences::instance( );
	m_host = prefs->host( );
	m_port = prefs->port( );
	m_secure = prefs->secure( );
	m_clientCertFile = prefs->clientCertFile( );
	m_clientKeyFile = prefs->clientKeyFile( );
	m_CACertFile = prefs->caCertFile( );
	m_uiLoadMode = prefs->uiLoadMode( );
	m_dataLoadMode = prefs->dataLoadMode( );
	m_dbName = prefs->dbName( );
	m_debug = prefs->debug( );
	m_uiFormsDir = prefs->uiFormsDir( );
	m_uiFormTranslationsDir = prefs->uiFormTranslationsDir( );
	m_dataLoaderDir = prefs->dataLoaderDir( );
	if( prefs->locale( ) == SYSTEM_LANGUAGE ) {
		m_language = QLocale::system( ).name( );
	} else {
		m_language = prefs->locale( );
	}
}

static DebugTerminal *debugTerminal = 0;

static void myMessageOutput( QtMsgType type, const char *msg )
{
	switch( type ) {
		case QtDebugMsg:
		case QtWarningMsg:
		case QtCriticalMsg:
		case QtFatalMsg:
			if( debugTerminal )
				debugTerminal->sendComment( msg );
			fprintf( stderr, "%s\n", msg );
			break;
			
		default:
			break;
	}
}

MainWindow::~MainWindow( )
{
	if( m_formWidget ) delete m_formWidget;
	if( m_debugTerminal ) {
		delete m_debugTerminal;
		debugTerminal = 0;
	}
	if( m_wolframeClient ) delete m_wolframeClient;
	if( m_formLoader ) delete m_formLoader;
	if( m_dataLoader ) delete m_dataLoader;
	if( m_uiLoader ) delete m_uiLoader;

	{
		QSqlDatabase db = QSqlDatabase::database( SESSION_NAME );
		db.rollback( );
		db.close( );
	}
	QSqlDatabase::removeDatabase( SESSION_NAME ); 	
}

void MainWindow::parseArgs( )
{
	const struct QCommandLineConfigEntry conf[] =
	{
		{ QCommandLine::Option, 's', "settings", "Use settings from this file", QCommandLine::Optional },
		{ QCommandLine::Option, 'H', "host", "Wolframe host", QCommandLine::Optional },
		{ QCommandLine::Option, 'p', "port", "Wolframe port", QCommandLine::Optional },
		{ QCommandLine::Switch, 'S', "secure", "connect securely via SSL", QCommandLine::Optional },
		{ QCommandLine::Switch, 'f', "ui-local-file", "Run with local form loader (in filesystem)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'l', "ui-local-db", "Run with local dform loader (in sqllite DB)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'n', "ui-network", "Run with network storage for forms", QCommandLine::Optional },
		{ QCommandLine::Switch, 'F', "data-local-file", "Run with local form loader (in filesystem)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'L', "data-local-db", "Run with local data loader (in sqllite DB)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'N', "data-network", "Run with network storage for data", QCommandLine::Optional },
		{ QCommandLine::Switch, 'd', "debug", "Enable debug window when starting", QCommandLine::Optional },
		{ QCommandLine::Option, 'v', "verbose", "verbose level", QCommandLine::Optional },
		{ QCommandLine::Option, 'c', "client-cert-file", "client certificate to present to the server (default: ./certs/client.crt)", QCommandLine::Optional },
		{ QCommandLine::Option, 'k', "client-key-file", "client key file (default: ./private/client.key)", QCommandLine::Optional },
		{ QCommandLine::Option, 'C', "CA-cert-file", "certificate file containing the CA (default: ./certs/CAclient.cert.pem)", QCommandLine::Optional },
		{ QCommandLine::Option, 'D', "db-file", "Sqlite3 file for local storage", QCommandLine::Optional },
		QCOMMANDLINE_CONFIG_ENTRY_END
	};

	if( !m_cmdline ) {
		m_cmdline = new QCommandLine( this );
		m_cmdline->setConfig( conf );
		m_cmdline->enableVersion( true );
		m_cmdline->enableHelp( true );

		connect( m_cmdline, SIGNAL( switchFound( const QString & ) ),
			this, SLOT( switchFound( const QString & ) ) );

		connect( m_cmdline, SIGNAL( optionFound( const QString &, const QVariant & ) ),
			this, SLOT( optionFound( const QString &, const QVariant & ) ) );

		connect( m_cmdline, SIGNAL( paramFound( const QString &, const QVariant & ) ),
			this, SLOT( paramFound( const QString &, const QVariant & ) ) );

		connect( m_cmdline, SIGNAL( parseError( const QString & ) ),
			this, SLOT( parseError( const QString & ) ) );
	}
	
	m_cmdline->parse( );
}

void MainWindow::switchFound( const QString &name )
{
	qDebug( ) << "switch" << name;
	if( name == "ui-local-file" ) {
		m_uiLoadMode = LocalFile;
	} else if( name == "ui-local-db" ) {
		m_uiLoadMode = LocalDb;
	} else if( name == "ui-network" ) {
		m_uiLoadMode = Network;
	} else if( name == "data-local-file" ) {
		m_dataLoadMode = LocalFile;
	} else if( name == "data-local-db" ) {
		m_dataLoadMode = LocalDb;
	} else if( name == "data-network" ) {
		m_dataLoadMode = Network;
	} else if( name == "secure" ) {
		m_secure = true;
	} else if( name == "debug" ) {
		m_debug = true;
	}
}

void MainWindow::optionFound( const QString &name, const QVariant &value )
{
	qDebug( ) << "option" << name << "with" << value;
	if( name == "settings" ) {
		m_settings = value.toString( );
	} else if( name == "host" ) {
		m_host = value.toString( );
	} else if( name == "port" ) {
		m_port = value.toString( ).toUShort( );
	} else if( name == "client-cert-file" ) {
		m_clientCertFile = value.toString( );
	} else if( name == "client-key-file" ) {
		m_clientKeyFile = value.toString( );
	} else if( name == "CA-cert-file" ) {
		m_CACertFile = value.toString( );
	} else if( name == "db-file" ) {
		m_dbName = value.toString( );
	}
}

void MainWindow::paramFound( const QString &name, const QVariant &value )
{
	qDebug( ) << "param" << name << "with" << value;
}

void MainWindow::parseError( const QString &error )
{
	qWarning( ) << qPrintable( error );
	m_cmdline->showHelp( true, -1 );
	QCoreApplication::quit( );
}

void MainWindow::initialize( )
{
// create a Wolframe protocol client
	m_wolframeClient = new WolframeClient( m_host, m_port, m_secure, m_clientCertFile, m_clientKeyFile, m_CACertFile );

// create debuging terminal
	m_debugTerminal = new DebugTerminal( m_wolframeClient, this );
	debugTerminal = m_debugTerminal;
#ifndef Q_OS_ANDROID
// crashes somehow on Android, must investigate
	qInstallMsgHandler( &myMessageOutput );
	if( m_debug ) m_debugTerminal->bringToFront( );
#endif
	qDebug( ) << "Debug window initialized";

// open local sqlite database
	if( m_uiLoadMode == LocalDb || m_dataLoadMode == LocalDb ) {
		QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", SESSION_NAME );
		db.setDatabaseName( m_dbName );
		if( !db.open( ) ) {
			QString msg = tr( "Unable to open or create Sqlite database file '%1'" ).
				arg( QFileInfo( m_dbName ).fileName( ) );
			QMessageBox::warning( this, "qtclient", msg );
			QCoreApplication::quit( );
		}
		
		QSqlQuery q( "select 1 from sqlite_master", db );
		if( !q.exec( ) ) {
			QString msg = tr( "Unable to query the Sqlite database file '%1', most likely not a database." ).
				arg( QFileInfo( m_dbName ).fileName( ) );
			QMessageBox::warning( this, "qtclient", msg );
			QCoreApplication::quit( );
		}

// enable foreign key
		QSqlQuery qp( "PRAGMA foreign_keys = ON", db );
		if( !qp.exec( ) ) {
			QString msg = tr( "Unable to enable foreign key support in the Sqlite database file '%1'." ).
				arg( QFileInfo( m_dbName ).fileName( ) );
			QMessageBox::warning( this, "qtclient", msg );
			QCoreApplication::quit( );
		}
	}
			
// catch error of network protocol
	connect( m_wolframeClient, SIGNAL( error( QString ) ),
		this, SLOT( wolframeError( QString ) ) );

// connect lines sent by anybody to the debug window
	connect( m_wolframeClient, SIGNAL( lineSent( QString ) ),
		m_debugTerminal, SLOT( sendLine( QString ) ) );
		
// a Qt UI loader for the main theme window and also used by all form widgets
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled( true );
//	m_uiLoader->addPluginPath( "plugins" );
	m_uiLoader->addPluginPath( "." );
	QStringList paths = m_uiLoader->pluginPaths( );
	qDebug( ) << "Will load custom widget plugins from" << paths;
	
// for testing, load lists of available forms from the files system or
// a local sqlite database, pass the form loader to the FormWidget
	switch( m_uiLoadMode ) {
		case LocalFile:
			m_formLoader = new FileFormLoader( m_uiFormsDir, m_uiFormTranslationsDir );
			break;
		
		case LocalDb:
			m_formLoader = new SqliteFormLoader( SESSION_NAME );
			break;
			
		case Network:
			// skip, delay
			break;
	}
			
// ..same for the data loader
	switch( m_dataLoadMode ) {
		case LocalFile:
			m_dataLoader = new FileDataLoader( m_dataLoaderDir );
			break;
		
		case LocalDb:
			m_dataLoader = new SqliteDataLoader( SESSION_NAME );
			break;
			
		case Network:
			// skip, delay
			break;
	}

// end of what we can do in network mode, initiate connect here and bail out
// connect the wolframe client to protocols, authenticate
	if( m_uiLoadMode == Network || m_dataLoadMode == Network ) {
		connect( m_wolframeClient, SIGNAL( connected( ) ),
			this, SLOT( connected( ) ) );
		connect( m_wolframeClient, SIGNAL( disconnected( ) ),
			this, SLOT( disconnected( ) ) );
		m_wolframeClient->connect( );
		return;
	}
	
	finishInitialize( );
}

void MainWindow::finishInitialize( )
{
// create delegate widget for form handling (one for now), in theory may are possible
	m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this );
	
// link the form loader for form loader notifications needed by the main window
// (list of forms for form menu, list of language for language picker)
	connect( m_formLoader, SIGNAL( languageCodesLoaded( QStringList ) ),
		this, SLOT( languageCodesLoaded( QStringList ) ) );
	connect( m_formLoader, SIGNAL( formListLoaded( QStringList ) ),
		this, SLOT( formListLoaded( QStringList ) ) );

// get notified if the form widget changes a form
	connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
		this, SLOT( formLoaded( QString ) ) );
// errors in the form widget
	connect( m_formWidget, SIGNAL( error( QString ) ),
		this, SLOT( formError( QString ) ) );
		
// set default language to the system language
	m_currentLanguage = m_language;

// load default theme
#ifdef Q_OS_ANDROID
	loadTheme( QString( QLatin1String( "phone" ) ) );
#else
	loadTheme( QString( QLatin1String( "windows" ) ) );
#endif

// load language resources, repaints the whole interface if necessary
	loadLanguage( m_currentLanguage );
	
// load initial form
	loadForm( "init" );
}

void MainWindow::connected( )
{
	m_loginDialog = new LoginDialog( m_wolframeClient, this );
	
	connect( m_loginDialog, SIGNAL( authenticationOk( ) ),
		this, SLOT( authenticationOk( ) ) );
		
	connect( m_loginDialog, SIGNAL( authenticationFailed( ) ),
		this, SLOT( authenticationFailed( ) ) );
		
	m_loginDialog->show( );
}	

void MainWindow::disconnected( )
{
	disconnect( m_wolframeClient, SIGNAL( error( QString ) ), 0, 0 );
	m_debugTerminal->close( );
	m_debugTerminal = 0;
	debugTerminal = 0;
		
	close( );
}

void MainWindow::wolframeError( QString error )
{
 	QMessageBox::information( this, tr( "Protocol error" ), error, QMessageBox::Ok );

// fatal error, present the user a preferences dialog
	if( !m_dataLoader || !m_formLoader ) {
		PreferencesDialog prefs( m_languages, this );
		if( prefs.exec( ) == QDialog::Accepted ) {
			qDebug( ) << "Reloading application";
			QApplication::instance( )->exit( RESTART_CODE );
		} else {
// fatal situation, terminate
			QApplication::instance( )->exit( 0 );
		}
	}
}

void MainWindow::authenticationOk( )
{
	m_loginDialog->close( );
	m_loginDialog->deleteLater( );
	
	qDebug( ) << "authentication succeeded";

// create network based form ...
	if( m_uiLoadMode == Network ) {
		m_formLoader = new NetworkFormLoader( m_wolframeClient );
	}
	
// ...and data loaders
	if( m_dataLoadMode == Network ) {
		m_dataLoader = new NetworkDataLoader( m_wolframeClient );
	}

	finishInitialize( );
}

void MainWindow::authenticationFailed( )
{
	m_loginDialog->close( );
	m_loginDialog->deleteLater( );

	qDebug( ) << "authentication failed";
	
	QApplication::instance( )->exit( RESTART_CODE );
}

void MainWindow::populateThemesMenu( )
{
// construct a menu which shows all available themes in a directory
	QDir themes_dir( QLatin1String( "themes" ) );
	QStringList themes = themes_dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
	QMenu *themes_menu = qFindChild<QMenu *>( m_ui, "menuThemes" );
	QActionGroup *themesGroup = new QActionGroup( themes_menu );
	themesGroup->setExclusive( true );
	foreach( QString t, themes ) {
		QAction *action = new QAction( t, themesGroup );
		action->setCheckable( true );
		themesGroup->addAction( action );
		if( t == m_currentTheme ) action->setChecked( true );
	}

// connect signal for theme selection
	themes_menu->addActions( themesGroup->actions( ) );
	connect( themesGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( themeSelected( QAction * ) ) );
}

void MainWindow::loadTheme( QString theme )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

// set working directory to theme
#ifdef Q_OS_ANDROID
	QString themesFolder( QLatin1String( "assets:/themes/" ) + theme + QLatin1Char( '/' ) );
#else
	QString themesFolder( QLatin1String( "themes/" ) + theme + QLatin1Char( '/' ) );
#endif

// tell the loader that this is the working directory
	m_uiLoader->setWorkingDirectory( themesFolder );

// remember current user interface
	QWidget *oldUi = m_ui;

// load the main window (which is empty) and provides basic functions like
// theme switching, login, exit, about, etc. (to start unauthenticated)
	QFile file( themesFolder + QLatin1String( "MainWindow.ui" ) );
	file.open( QFile::ReadOnly );
	m_ui = m_uiLoader->load( &file, this );
	file.close( );

// set stylesheet of the application (has impact on the whole application)
	QFile qss( themesFolder + QLatin1String( "MainWindow.qss" ) );
#ifdef Q_OS_ANDROID
	qApp->setStyleSheet( QFileInfo( qss ).filePath( ) );
#else
	qApp->setStyleSheet( QLatin1String( "file:///" ) + QFileInfo( qss ).absoluteFilePath( ) );
#endif
	
// copy over the location of the old window to the new one
// also copy over the current form, don't destroy the old ui,
// events could be outstanding (deleteLater marks the widget 
// for deletion, will be deleted when returning into the event
// loop), also set a new empty parent for the old theme, otherwise
// autowiring rewires the first widget it finds (which can be
// the one we want to delete)
	if( oldUi ) {
		m_ui->move( oldUi->pos( ) );
		oldUi->hide( );
		oldUi->deleteLater( );
		oldUi->setParent( 0 );
	}

// attach form widget in the right place of the theme main window
	QVBoxLayout *l = qFindChild<QVBoxLayout *>( m_ui, "mainAreaLayout" );
	l->addWidget( m_formWidget );

// show the new gui
	m_ui->show( );

// wire standard actions in the menu by name (on_<object>_<signal>)
	QMetaObject::connectSlotsByName( this );
	
// remember current theme
	m_currentTheme = theme;

// load all themes possible to pick and mark the current one
	populateThemesMenu( );

// now that we have a menu where we can add things, we start the form list loading
	m_formLoader->initiateListLoad( );

// load language codes for language picker
	loadLanguages( );
	
// not busy anymore
	qApp->restoreOverrideCursor( );

// load the current form again
	if( m_currentForm  != "" ) loadForm( m_currentForm );
}

void MainWindow::loadLanguages( )
{
// get the list of available languages
	m_formLoader->initiateGetLanguageCodes( );
}

void MainWindow::languageCodesLoaded( QStringList languages )
{
// remember languages for preferences dialog
	m_languages = languages;
	
// construct a menu showing all languages
	QMenu *languageMenu = qFindChild<QMenu *>( m_ui, "menuLanguages" );
	languageMenu->clear( );
	QActionGroup *languageGroup = new QActionGroup( languageMenu );
	languageGroup->setExclusive( true );
	foreach( QString language, languages ) {
		QLocale myLocale( language );
		QAction *action = new QAction( myLocale.languageToString( myLocale.language( ) ) + " (" + language + ")", languageGroup );
		action->setCheckable( true );
		action->setData( QVariant( language ) );
		languageGroup->addAction( action );
		if( language == m_currentLanguage ) action->setChecked( true );		
	}
	languageMenu->addActions( languageGroup->actions( ) );
	connect( languageGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( languageSelected( QAction * ) ) );
}

void MainWindow::formListLoaded( QStringList forms )
{
// contruct a menu which shows and wires them in the menu
	QMenu *formsMenu = qFindChild<QMenu *>( m_ui, "menuForms" );
	formsMenu->clear( );
	QActionGroup *formGroup = new QActionGroup( formsMenu );
	formGroup->setExclusive( true );
	foreach( QString form, forms ) {
		QAction *action = new QAction( form, formGroup );
		action->setCheckable( true );
		formGroup->addAction( action );
		if( form == m_currentForm ) action->setChecked( true );
	}
	formsMenu->addActions( formGroup->actions( ) );
	connect( formGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( formSelected( QAction * ) ) );
	
// not busy anymore
	qApp->restoreOverrideCursor();
}

void MainWindow::languageSelected( QAction *action )
{
	QString language = action->data( ).toString( );
	if( language != m_currentLanguage )
		loadLanguage( language );
}

void MainWindow::loadLanguage( QString language )
{		
	qDebug( ) << "Switching interface language to " << language;

// get list of all translators currently floating around and delete them
	const QList<QTranslator *> oldTranslators( findChildren<QTranslator *>( ) );
	foreach( QTranslator *translator, oldTranslators ) {
		QCoreApplication::instance( )->removeTranslator( translator );
	}
	qDeleteAll( oldTranslators );

// this it the default language, bail out as no translations are necessary
	if( language == DEFAULT_LOCALE ) {
		m_currentLanguage = language;
		return;
	}

// install new ones, first the ones of the theme, then the ones of the current form
// all other forms will reinstall the correct language when called again
	QTranslator *translator = new QTranslator( this );
	
	if( !translator->load( "qtclient." + language, "i18n/" + m_currentTheme ) ) {
		qDebug( ) << "Error while loading translations for qtclient " <<
			m_currentTheme << " for locale " << language;
	}
	QCoreApplication::instance( )->installTranslator( translator );

	translator = new QTranslator( this );
	if( !translator->load( "MainWindow." + language, "themes/" + m_currentTheme ) ) {
		qDebug( ) << "Error while loading translations for theme " <<
			m_currentTheme << " for locale " << language;
	}
	QCoreApplication::instance( )->installTranslator( translator );

// also set language of the form widget
	m_formWidget->loadLanguage( language );
	
	m_currentLanguage = language;
}

void MainWindow::themeSelected( QAction *action )
{
	QString theme = action->text( );
	if( theme != m_currentTheme )
		loadTheme( theme );
}

void MainWindow::formSelected( QAction *action )
{		
	QString form = action->text( );
	qDebug( ) << "Form " << form << " selected (current form:" << m_currentForm << ")";
	if( form != m_currentForm )
		loadForm( form );
}

void MainWindow::loadForm( QString name )
{
// delegate form loading to form widget
	m_formWidget->loadForm( name );	
}

void MainWindow::formLoaded( QString name )
{
// remember the name of the current form
	m_currentForm = name;
	
// also set language of the form widget,
// but wait till the form got loaded, otherwise we get races!
	m_formWidget->loadLanguage( m_currentLanguage );

// make sure the correct action is checked in the menu of forms
	QMenu *formsMenu = qFindChild<QMenu *>( m_ui, "menuForms" );
	QList<QAction *> _actions = formsMenu->findChildren<QAction *>( );
	foreach( QAction *action, _actions ) {
		if( action->text( ) == name ) action->setChecked( true );
	}
}

void MainWindow::formError( QString error )
{
	qDebug( ) << "Form error: " << error;
 	
// not busy anymore
	qApp->restoreOverrideCursor();

	QMessageBox::information( this, tr( "Form error" ), error,QMessageBox::Ok );
}

void MainWindow::on_actionRestart_triggered( )
{
	qDebug( ) << "Restarting application";
	QApplication::instance( )->exit( RESTART_CODE );
}

void MainWindow::on_actionExit_triggered( )
{	
	if( m_uiLoadMode == Network || m_dataLoadMode == Network ) {
		m_wolframeClient->disconnect( );
	} else {
// terminate brutally in local mode	
		disconnect( m_wolframeClient, SIGNAL( error( QString ) ), 0, 0 );
		m_debugTerminal = 0;
		debugTerminal = 0;
		
		close( );
	}
}

void MainWindow::on_actionPreferences_triggered( )
{
	PreferencesDialog prefs( m_languages, this );
	if( prefs.exec( ) == QDialog::Accepted ) {
		qDebug( ) << "Reloading application";
		QApplication::instance( )->exit( RESTART_CODE );
	}
}

void MainWindow::on_actionManageStorage_triggered( )
{
	ManageStorageDialog d( this );
	d.exec( );
}

void MainWindow::on_actionAbout_triggered( )
{
	QString info = QString( tr( "qtclient" ) );
	QMessageBox::about( m_ui, tr( "qtclient" ), info );
}

void MainWindow::on_actionAboutQt_triggered( )
{
	QMessageBox::aboutQt( m_ui, tr( "qtclient" ) );
}

void MainWindow::on_actionDebugTerminal_triggered( bool checked )
{
	if( checked ) {
		m_debugTerminal->bringToFront( );
	} else {
		m_debugTerminal->hide( );
	}
}

void MainWindow::setOrientation( ScreenOrientation orientation )
{
	Qt::WidgetAttribute attribute;
	switch( orientation ) {
#if QT_VERSION < 0x040702
// Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
		case ScreenOrientationLockPortrait:
			attribute = static_cast<Qt::WidgetAttribute>( 128 );
			break;

		case ScreenOrientationLockLandscape:
			attribute = static_cast<Qt::WidgetAttribute>( 129 );
			break;

		case ScreenOrientationAuto:
		default:
			attribute = static_cast<Qt::WidgetAttribute>( 130 );
			break;
#else // QT_VERSION < 0x040702
		case ScreenOrientationLockPortrait:
			attribute = Qt::WA_LockPortraitOrientation;
			break;
			
		case ScreenOrientationLockLandscape:
			attribute = Qt::WA_LockLandscapeOrientation;
			break;

		default:
			case ScreenOrientationAuto:
			attribute = Qt::WA_AutoOrientation;
			break;
#endif // QT_VERSION < 0x040702
	}
	
	setAttribute( attribute, true );
}

void MainWindow::showExpanded( )
{
#if defined( Q_WS_SIMULATOR )
	showFullScreen( );
#elif defined( Q_WS_MAEMO_5 )
	showMaximized( );
#else
	show( );
#endif
}


