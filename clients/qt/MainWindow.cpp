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
#include "manageServersDialog.hpp"

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
	m_languages( ), m_language( ), m_mdiArea( 0 ), m_mdi( false ),
	m_subWinGroup( 0 )
{
// setup designer UI
	m_ui.setupUi( this );
	
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
	m_mdi = prefs->mdi( );

	settings.read( "Wolframe", "Wolframe Client" );
	move( settings.mainWindowPos );
	resize( settings.mainWindowSize );
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
	if( m_mdi ) {
		if( m_mdiArea )
			m_mdiArea->closeAllSubWindows( );
	} else {
		if( m_formWidget ) {
			delete m_formWidget;
			m_formWidget = 0;
		}
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
// link the form loader for form loader notifications needed by the main window
// (list of forms for form menu, list of language for language picker)
	connect( m_formLoader, SIGNAL( languageCodesLoaded( QStringList ) ),
		this, SLOT( languageCodesLoaded( QStringList ) ) );
	connect( m_formLoader, SIGNAL( formListLoaded( QStringList ) ),
		this, SLOT( formListLoaded( QStringList ) ) );

// set default language to the system language
	m_currentLanguage = m_language;

// create central widget, either as MDI area or as one form widget
	if( m_mdi ) {
		m_mdiArea = findChild<QMdiArea *>( "centralWidget" );
		if( m_mdiArea ) {
// attach mdi area to some specific signals
			m_mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
			m_mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
			connect( m_mdiArea, SIGNAL( subWindowActivated( QMdiSubWindow * ) ),
				this, SLOT( subWindowChanged( QMdiSubWindow * ) ) );
							
// connect some MDI specific signals to menu and toolbar
			QAction *action = findChild<QAction *>( "actionTile" );
			if( action ) {
				action->setEnabled( true );
				connect( action, SIGNAL( triggered( ) ),
					m_mdiArea, SLOT( tileSubWindows( ) ) );
			}
			action = findChild<QAction *>( "actionCascade" );
			if( action ) {
				action->setEnabled( true );
				connect( action, SIGNAL( triggered( ) ),
					m_mdiArea, SLOT( cascadeSubWindows( ) ) );
			}

// default is tiling (TODO: remeber position, forms and sizes of open windows)
			m_mdiArea->tileSubWindows( );

// create initial sub window (TODO: remeber position, forms and sizes of open windows)
			CreateMdiSubWindow( "init" );
		} else {
// missing a MDI araa, so add just one form widget as main entry widget
			m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, m_debug );

			connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
				this, SLOT( formLoaded( QString ) ) );
			connect( m_formWidget, SIGNAL( error( QString ) ),
				this, SLOT( formError( QString ) ) );

			setCentralWidget( m_formWidget );
			m_mdi = false;
		}
	} else {
// non-MDI mode, open one form
		m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, m_debug );

		connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
			this, SLOT( formLoaded( QString ) ) );
		connect( m_formWidget, SIGNAL( error( QString ) ),
			this, SLOT( formError( QString ) ) );
		
		setCentralWidget( m_formWidget );
	}

// make debug action available
	activateAction( "actionDebugTerminal", m_debug );

// update menus and toolbars
	updateMenusAndToolbars( );

// enable open forms in new window in MDI mode
	activateAction( "actionOpenFormNewWindow", m_mdi );

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
// get the list of available languages in the forms
	m_formLoader->initiateGetLanguageCodes( );
}

void MainWindow::languageCodesLoaded( QStringList languages )
{
	QStringList languageCodes;
	languageCodes.push_back( DEFAULT_LOCALE ); // default locale, always around
	
// read list of supported languages based on the qtclient translations
	QDir translationDir( "i18n" );
	translationDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
	translationDir.setSorting( QDir::Name );
	QStringList filters;
	filters << "*.qm";
	translationDir.setNameFilters( filters );
	QStringList localeFiles = translationDir.entryList( );
	QMutableStringListIterator it( localeFiles );
	while( it.hasNext( ) ) {
		it.next( );
		QStringList parts = it.value( ).split( "." );
		languageCodes.push_back( parts[1] );		
	}
	
// add the ones supported in forms
	languageCodes.append( languages );
	languageCodes.removeDuplicates( );		

// remember languages for preferences dialog
	m_languages = languageCodes;

// does the menu exist?
	QMenu *languageMenu = qFindChild<QMenu *>( this, "menuLanguages" );
	if( !languageMenu ) return;
	
// construct a menu showing all languages
	languageMenu->clear( );
	QActionGroup *languageGroup = new QActionGroup( languageMenu );
	languageGroup->setExclusive( true );
	foreach( QString language, m_languages ) {
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

void MainWindow::languageSelected( QAction *action )
{
	QString language = action->data( ).toString( );
	if( language != m_currentLanguage )
		loadLanguage( language );
}

void MainWindow::switchTranslator( QTranslator &translator, const QString &filename, const QString &i18n )
{
	qApp->removeTranslator( &translator );
	
	if( translator.load( filename, i18n ) ) {
		qApp->installTranslator( &translator );
	} else {
		qWarning( ) << "Error while loading translations for" << filename << "from directory" << i18n;
	}
}

void MainWindow::loadLanguage( QString language )
{
	qDebug( ) << "Switching interface language to " << language;

// change language on global level
	switchTranslator( m_translatorApp, QString( "qtclient.%1.qm" ).arg( language ), "i18n" );
	switchTranslator( m_translatorQt, QString( "qt_%1.qm" ).arg( language ), "/usr/share/qt/translations/" );
        
// also set language of the form widget
	if( m_formWidget )
		m_formWidget->loadLanguage( language );

	m_currentLanguage = language;
}

void MainWindow::changeEvent( QEvent* _event )
{
	if( _event ) {
		switch( _event->type( ) ) {
			
			case QEvent::LanguageChange:
				m_ui.retranslateUi( this );
				break;

			case QEvent::LocaleChange:
			{
				QString locale = QLocale::system( ).name( );
				locale.truncate( locale.lastIndexOf( '_' ) );
				loadLanguage( locale );
			}
			break;
		}
	}

	QMainWindow::changeEvent( _event );
}

void MainWindow::formListLoaded( QStringList forms )
{
	m_forms = forms;
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
			QString title = m_formWidget->windowTitle( );
			mdiSubWindow->setWindowTitle( title );
			QAction *action = m_revSubWinMap.value( mdiSubWindow );
			if( action ) {
				int idx = action->data( ).toInt( );
				QString text = composeWindowListTitle( idx, title );
				action->setText( text );
			}
		}
	} else {
		setWindowTitle( tr( "Wolframe Qt Client - %1" ).arg( m_formWidget->windowTitle( ) ) );
	}
}

QString MainWindow::composeWindowListTitle( const int idx, const QString title )
{
	QString text;
	
	if( idx < 10 ) {
		text = tr( "&%1 %2" ).arg( idx ).arg( title );
	} else {
		text = tr( "%1 %2" ).arg( idx ).arg( title );
	}
	
	return text;
}			

void MainWindow::updateWindowMenu( )
{
	QMdiSubWindow *mdiSubWindow = m_mdiArea->activeSubWindow( );
	QAction *action = m_revSubWinMap.value( mdiSubWindow );
	if( action )
		action->setChecked( true );
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

void MainWindow::closeEvent( QCloseEvent *e )
{
	settings.mainWindowPos = pos( );
	settings.mainWindowSize = size( );
	settings.write( "Wolframe", "Wolframe Client" );
	e->accept( );
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
	QMessageBox aboutBox( this );
	aboutBox.setParent( this );
	QString info = QString(
		tr( "Wolframe Qt client\n\nVersion %1\n(c) 2012, 2013 Wolframe Group\n\nBuild: %2 %3" )
			.arg( WOLFRAME_VERSION )
			.arg( __DATE__ )
			.arg( __TIME__ ) );
	aboutBox.setText( info );
	aboutBox.setIconPixmap( QPixmap( QString( ":/images/wolfClient.png" ) ) );
	aboutBox.exec( );
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

void MainWindow::CreateMdiSubWindow( const QString form )
{
	FormWidget *formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, m_debug );

	connect( formWidget, SIGNAL( formLoaded( QString ) ),
		this, SLOT( formLoaded( QString ) ) );
	connect( formWidget, SIGNAL( error( QString ) ),
		this, SLOT( formError( QString ) ) );
	connect( formWidget,SIGNAL( destroyed( ) ),
		this, SLOT( updateMenusAndToolbars( ) ) );

	QMdiSubWindow *mdiSubWindow = m_mdiArea->addSubWindow( formWidget );
	mdiSubWindow->setAttribute( Qt::WA_DeleteOnClose );

	formWidget->show( );
	loadForm( form );

	mdiSubWindow->resize( mdiSubWindow->sizeHint( ) );
}

void MainWindow::on_actionOpenFormNewWindow_triggered( )
{
	FormChooseDialog d( m_forms, this );
	if( d.exec( ) == QDialog::Accepted ) {
		CreateMdiSubWindow( d.form( ) );
	}

	updateMenusAndToolbars( );
}

void MainWindow::subWindowSelected( QAction *action )
{
	QMdiSubWindow *w = m_subWinMap.value( action );
	m_mdiArea->setActiveSubWindow( w );
}

void MainWindow::subWindowChanged( QMdiSubWindow *w )
{
	if( !w ) return;

	m_formWidget = qobject_cast<FormWidget *>( w->widget( ) );

	updateWindowMenu( );
}

void MainWindow::on_actionReloadWindow_triggered( )
{
	m_formWidget->reload( );
}

void MainWindow::on_actionNextWindow_triggered( )
{
	m_mdiArea->activateNextSubWindow( );

	updateWindowMenu( );
}

void MainWindow::on_actionPreviousWindow_triggered( )
{
	m_mdiArea->activatePreviousSubWindow( );
	
	updateWindowMenu( );
}

void MainWindow::on_actionClose_triggered( )
{
	m_mdiArea->closeActiveSubWindow( );

	updateMenusAndToolbars( );
}

void MainWindow::on_actionCloseAll_triggered( )
{
	m_mdiArea->closeAllSubWindows( );

	updateMenusAndToolbars( );
}

int MainWindow::nofSubWindows( ) const
{
	QList<QMdiSubWindow *> list = m_mdiArea->subWindowList( );
	return list.count( );
}

void MainWindow::activateAction( const QString name, bool enabled )
{
	QAction *action = findChild<QAction *>( name );
	if( action ) {
		action->setEnabled( enabled );
	}
}

void MainWindow::updateMenusAndToolbars( )
{
	if( m_mdi ) {
// enable/disable menu/toolbar items depending on the number of subwindows
		activateAction( "actionClose", nofSubWindows( ) > 0 );
		activateAction( "actionCloseAll", nofSubWindows( ) > 0 );
		activateAction( "actionNextWindow", nofSubWindows( ) > 1 );
		activateAction( "actionPreviousWindow", nofSubWindows( ) > 1 );
		activateAction( "actionTile", nofSubWindows( ) > 0 );
		activateAction( "actionCascade", nofSubWindows( ) > 0 );		

// recreate the subwindow menu and mark the currently selected submenu
		QMenu *windowMenu = qFindChild<QMenu *>( this, "menuWindow" );
		if( windowMenu ) {
			m_subWinMap.clear( );
			m_revSubWinMap.clear( );
			
			if( m_subWinGroup ) {
				foreach( QAction *action, m_subWinGroup->actions( ) ) {
					m_subWinGroup->removeAction( action );
					delete action;
				}
			}
			
			m_subWinGroup = new QActionGroup( windowMenu );
			m_subWinGroup->setExclusive( true );
			QAction *action = new QAction( "", m_subWinGroup );
			action->setSeparator( true );
			m_subWinGroup->addAction( action );
			int idx = 1;
			foreach( QMdiSubWindow *w, m_mdiArea->subWindowList( ) ) {
				// TODO: translation is tricky here: the form widget has
				// a QTranslator class and can call translate on it using
				// the translation resource of the corresponding form!
				// see QString QTranslator::translate ( const char * context, const char * sourceText, const char * disambiguation, int n ) const
				QString title = w->windowTitle( );
				QString text = composeWindowListTitle( idx, title );
				action = new QAction( text, m_subWinGroup );
				action->setCheckable( true );
				action->setData( QVariant( idx ) );
				m_subWinGroup->addAction( action );
				if( w == m_mdiArea->activeSubWindow( ) ) {
					action->setChecked( true );
				}
				m_subWinMap.insert( action, w );
				m_revSubWinMap.insert( w, action );
				idx++;
			}
			windowMenu->addActions( m_subWinGroup->actions( ) );
			
			connect( m_subWinGroup, SIGNAL( triggered( QAction * ) ),
				this, SLOT( subWindowSelected( QAction * ) ) );
		}
	}
}

void MainWindow::on_actionManageServers_triggered( )
{
	ManageServersDialog* serversDlg = new ManageServersDialog( settings.connectionParams );
	serversDlg->exec( );

	delete serversDlg;
}

