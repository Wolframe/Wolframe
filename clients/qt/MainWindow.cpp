//
// MainWindow.cpp
//

#include "MainWindow.hpp"
#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"
#include "NetworkFormLoader.hpp"
#include "NetworkDataLoader.hpp"
#include "Preferences.hpp"
#include "PreferencesDialog.hpp"
#include "global.hpp"
#include "FormChooseDialog.hpp"

#include <QtGui>
#include <QBuffer>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QtAlgorithms>
#include <QMessageBox>
#include <QDebug>

// built-in defaults
MainWindow::MainWindow( QWidget *_parent ) : QMainWindow( _parent ),
	m_cmdline( 0 ),
	m_formWidget( 0 ), m_uiLoader( 0 ), m_formLoader( 0 ),
	m_dataLoader( 0 ), m_debugTerminal( 0 ), m_wolframeClient( 0 ),
	m_host( "localhost" ), m_port( 7661 ), m_secure( false ), m_checkSSL( true ),
	m_clientCertFile( "./certs/client.crt" ), m_clientKeyFile( "./private/client.key" ),
	m_CACertFile( "./certs/CAclient.cert.pem" ),
	m_uiLoadMode( DEFAULT_UILOADMODE ), m_dataLoadMode( DEFAULT_DATALOADMODE ), m_debug( false ),
	m_loginDialog( 0 ), m_settings( ),
	m_uiFormsDir( DEFAULT_UI_FORMS_DIR ),
	m_uiFormTranslationsDir( DEFAULT_UI_FORM_TRANSLATIONS_DIR ),
	m_uiFormResourcesDir( DEFAULT_UI_FORM_RESOURCES_DIR ),
	m_dataLoaderDir( DEFAULT_DATA_LOADER_DIR ),
	m_languages( ), m_language( ), m_mdi( false )
{
// setup designer UI
	setupUi( this );
	
// read arguments for the '-s <setting file>' parameter
	parseArgs( );

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
	parseArgs( );

	initialize( );
}

static bool _debug = false;

void MainWindow::readSettings( )
{
	Preferences *prefs = Preferences::instance( );
	m_host = prefs->host( );
	m_port = prefs->port( );
	m_secure = prefs->secure( );
	m_checkSSL = prefs->checkSSL( );
	m_clientCertFile = prefs->clientCertFile( );
	m_clientKeyFile = prefs->clientKeyFile( );
	m_CACertFile = prefs->caCertFile( );
	m_uiLoadMode = prefs->uiLoadMode( );
	m_dataLoadMode = prefs->dataLoadMode( );
	m_debug = prefs->debug( );
	_debug = m_debug;
	m_uiFormsDir = prefs->uiFormsDir( );
	m_uiFormTranslationsDir = prefs->uiFormTranslationsDir( );
	m_uiFormResourcesDir = prefs->uiFormResourcesDir( );
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
			if( _debug ) {
				if( debugTerminal ) {
					debugTerminal->sendComment( msg );
				}
				fprintf( stderr, "%s\n", msg );
			}
			break;

		case QtWarningMsg:
			fprintf( stderr, "WARNING: %s\n", msg );
			break;

		case QtCriticalMsg:
			fprintf( stderr, "CRITICAL: %s\n", msg );
			break;

		case QtFatalMsg:
			fprintf( stderr, "FATAL: %s\n", msg );
			break;

		default:
			break;
	}
}

MainWindow::~MainWindow( )
{
	if( m_formWidget ) {
		delete m_formWidget;
		m_formWidget = 0;
	}
	if( m_debugTerminal ) {
		delete m_debugTerminal;
		debugTerminal = 0;
	}
	if( m_wolframeClient ) {
		delete m_wolframeClient;
		m_wolframeClient = 0;
	}
	if( m_formLoader ) {
		delete m_formLoader;
		m_formLoader = 0;
	}
	if( m_dataLoader ) {
		delete m_dataLoader;
		m_dataLoader = 0;
	}
	if( m_uiLoader ) {
		delete m_uiLoader;
		m_uiLoader = 0;
	}
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
		{ QCommandLine::Switch, 'n', "ui-network", "Run with network storage for forms", QCommandLine::Optional },
		{ QCommandLine::Switch, 'F', "data-local-file", "Run with local form loader (in filesystem)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'N', "data-network", "Run with network storage for data", QCommandLine::Optional },
		{ QCommandLine::Switch, 'd', "debug", "Enable debug window when starting", QCommandLine::Optional },
		{ QCommandLine::Option, 'v', "verbose", "verbose level", QCommandLine::Optional },
		{ QCommandLine::Option, 'c', "client-cert-file", "client certificate to present to the server (default: ./certs/client.crt)", QCommandLine::Optional },
		{ QCommandLine::Option, 'k', "client-key-file", "client key file (default: ./private/client.key)", QCommandLine::Optional },
		{ QCommandLine::Option, 'C', "CA-cert-file", "certificate file containing the CA (default: ./certs/CAclient.cert.pem)", QCommandLine::Optional },
		{ QCommandLine::Switch, 'm', "mdi", "MDI interface instead of one main window", QCommandLine::Optional },
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
	} else if( name == "ui-network" ) {
		m_uiLoadMode = Network;
	} else if( name == "data-local-file" ) {
		m_dataLoadMode = LocalFile;
	} else if( name == "data-network" ) {
		m_dataLoadMode = Network;
	} else if( name == "secure" ) {
		m_secure = true;
	} else if( name == "debug" ) {
		m_debug = true;
		_debug = true;
	} else if( name == "mdi" ) {
		m_mdi = true;
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
	m_wolframeClient = new WolframeClient( m_host, m_port, m_secure, m_checkSSL, m_clientCertFile, m_clientKeyFile, m_CACertFile );

// create debuging terminal (only if debugging is enable per command line or in preferences! Don't
// fill up the plain text widget with images!)
	if( m_debug ) {
		m_debugTerminal = new DebugTerminal( m_wolframeClient, this );
		debugTerminal = m_debugTerminal;

// connect lines sent by anybody to the debug window
		connect( m_wolframeClient, SIGNAL( lineSent( QString ) ),
			m_debugTerminal, SLOT( sendLine( QString ) ) );

		qDebug( ) << "Debug window initialized";
	}

// install custom output handler
	qInstallMsgHandler( &myMessageOutput );
	//if( m_debug ) m_debugTerminal->bringToFront( );

// catch error of network protocol
	connect( m_wolframeClient, SIGNAL( error( QString ) ),
		this, SLOT( wolframeError( QString ) ) );

// a Qt UI loader for the main theme window and also used by all form widgets
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled( true );
	m_uiLoader->addPluginPath( "plugins" );
	m_uiLoader->addPluginPath( "." );
	QStringList paths = m_uiLoader->pluginPaths( );
	qDebug( ) << "Will load custom widget plugins from" << paths;

// for testing, load lists of available forms from the files system or
// a local sqlite database, pass the form loader to the FormWidget
	switch( m_uiLoadMode ) {
		case LocalFile:
			m_formLoader = new FileFormLoader( m_uiFormsDir, m_uiFormTranslationsDir, m_uiFormResourcesDir );
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
	m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, m_debug );

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

	if( m_mdi ) {
		m_mdiArea = findChild<QMdiArea *>( "centralWidget" );
		if( m_mdiArea ) {
			m_mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
			m_mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
			connect( m_mdiArea, SIGNAL( subWindowActivated( QMdiSubWindow * ) ),
				this, SLOT( subWindowChanged( QMdiSubWindow * ) ) );
				
			QMdiSubWindow *mdiSubWindow = m_mdiArea->addSubWindow( m_formWidget, Qt::FramelessWindowHint );
			mdiSubWindow->setWindowState( Qt::WindowMaximized );
			mdiSubWindow->setAttribute( Qt::WA_DeleteOnClose );
			m_mdiArea->tileSubWindows( );
			mdiSubWindow->showMaximized( );
		} else {
			// missing a MDI aera, so add just one form widget as main entry widget
			setCentralWidget( m_formWidget );
		}
	} else {
		setCentralWidget( m_formWidget );
	}

// make debug action available
	if( !m_debug ) {
		QAction *action = findChild<QAction *>( "actionDebugTerminal" );
		if( action ) {
			action->setEnabled( false );
		}
	}

// enable open forms in new window in MDI mode
	QAction *action = findChild<QAction *>( "actionOpenFormNewWindow" );
	if( action ) {
		action->setEnabled( m_mdi );
	}

// now that we have a menu where we can add things, we start the form list loading
	m_formLoader->initiateListLoad( );

// load language codes for language picker
	loadLanguages( );

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
	if( m_debugTerminal ) {
		m_debugTerminal->close( );
	}
	m_debugTerminal = 0;
	debugTerminal = 0;

	close( );
}

void MainWindow::wolframeError( QString error )
{
// fatal error, present the user a choice whether to stop now or reconfigure
// the system
	if( !m_dataLoader || !m_formLoader ) {
		if( QMessageBox::critical( this, tr( "Protocol error, reconfigure now?" ),
			tr( "Protocol error: %1, reconfigure client now?" ).arg( error ),
			QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {

			PreferencesDialog prefs( m_languages, this );
			if( prefs.exec( ) == QDialog::Accepted ) {
// reload and use new settings
				qDebug( ) << "Reloading application";
				QApplication::instance( )->exit( RESTART_CODE );
			} else {
// fatal situation, terminate
				QApplication::instance( )->exit( 0 );
			}
		}
	} else {
// the error is normal way of life, so show only a normal error message in the statusbar
		statusBar( )->showMessage( error, 6000 );
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
		m_dataLoader = new NetworkDataLoader( m_wolframeClient, m_debug );
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
	QMenu *languageMenu = qFindChild<QMenu *>( this, "menuLanguages" );
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
	m_forms = forms;
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
	if( language != "en_US" ) {
		if( !translator->load( "qtclient." + language, "i18n" ) ) {
			qCritical( ) << "Error while loading translations for qtclient for locale " << language;
		}
		QCoreApplication::instance( )->installTranslator( translator );

		translator = new QTranslator( this );
		if( !translator->load( "MainWindow." + language, "themes/windows" ) ) {
			qCritical( ) << "Error while loading translations for locale " << language;
		}
		QCoreApplication::instance( )->installTranslator( translator );
	}

// also set language of the form widget
	if( m_formWidget )
		m_formWidget->loadLanguage( language );

	m_currentLanguage = language;
}

void MainWindow::loadForm( QString name )
{
// delegate form loading to form widget
	if( m_formWidget )
		m_formWidget->loadForm( name );
}

void MainWindow::formLoaded( QString name )
{
// also set language of the form widget,
// but wait till the form got loaded, otherwise we get races!
	m_formWidget->loadLanguage( m_currentLanguage );
	
// in MDI mode update the title of the sub window, otherwise update window title
	if( m_mdi ) {
		QMdiSubWindow *mdiSubWindow = m_mdiArea->activeSubWindow( );
		if( mdiSubWindow ) {
			mdiSubWindow->setWindowTitle( name );
		}
	} else {
		setWindowTitle( tr( "Wolframe Qt Client - %1" ).arg( name ) );
	}
}

void MainWindow::formError( QString error )
{
	qDebug( ) << "Form error: " << error;

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

void MainWindow::on_actionAbout_triggered( )
{
	QString info = QString(
		tr( "Wolframe Qt client\n\nVersion %1\n(c) 2012, 2013 Wolframe Group\n\nBuild: %2 %3" )
			.arg( WOLFRAME_VERSION )
			.arg( __DATE__ )
			.arg( __TIME__ ) );
	QMessageBox::about( this, tr( "About" ), info );
}

void MainWindow::on_actionAboutQt_triggered( )
{
	QMessageBox::aboutQt( this, tr( "qtclient" ) );
}

void MainWindow::on_actionDebugTerminal_triggered( bool checked )
{
	if( m_debugTerminal ) {
		if( checked ) {
			m_debugTerminal->bringToFront( );
		} else {
			m_debugTerminal->hide( );
		}
	}
}

void MainWindow::on_actionOpenForm_triggered( )
{
	FormChooseDialog d( m_forms, this );
	if( d.exec( ) == QDialog::Accepted ) {
		QString form = d.form( );
		loadForm( form );
	}
}

void MainWindow::on_actionOpenFormNewWindow_triggered( )
{
	FormChooseDialog d( m_forms, this );
	if( d.exec( ) == QDialog::Accepted ) {
		FormWidget *formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, m_debug );
		connect( formWidget, SIGNAL( formLoaded( QString ) ),
			this, SLOT( formLoaded( QString ) ) );
		connect( formWidget, SIGNAL( error( QString ) ),
			this, SLOT( formError( QString ) ) );

		QMdiSubWindow *mdiSubWindow = m_mdiArea->addSubWindow( formWidget );
		mdiSubWindow->setAttribute( Qt::WA_DeleteOnClose );

		QString form = d.form( );
		formWidget->show( );
		loadForm( form );
	}
}

void MainWindow::subWindowChanged( QMdiSubWindow *w )
{
	if( !w ) {
		return;
	}

	m_formWidget = qobject_cast<FormWidget *>( w->widget( ) );
}

