//
// MainWindow.cpp
//

#include "MainWindow.hpp"
#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"
#include "NetworkFormLoader.hpp"
#include "NetworkDataLoader.hpp"
#include "LoginDialog.hpp"

#include <QtGui>
#include <QBuffer>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QtAlgorithms>
#include <QMessageBox>

#include <QDebug>

namespace _Wolframe {
	namespace QtClient {

MainWindow::MainWindow( QWidget *_parent ) : QWidget( _parent ),
	m_ui( 0 ), m_host( "localhost" ), m_port( 7661 ), m_secure( false ),
	m_clientCertFile( "./certs/client.crt" ), m_clientKeyFile( "./private/client.key" ),
	m_CACertFile( "./certs/CAclient.cert.pem" ),
	m_loadMode( Network )
{
	parseArgs( );
	initialize( );
}

MainWindow::~MainWindow( )
{
	delete m_formWidget;
	delete m_debugTerminal;
	delete m_wolframeClient;
	delete m_formLoader;
	delete m_uiLoader;
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
				debugTerminal->sendLine( msg );
			fprintf( stderr, "%s\n", msg );
			break;
			
		default:
			break;
	}
}

void MainWindow::parseArgs( )
{
	const struct QCommandLineConfigEntry conf[] =
	{
		{ QCommandLine::Option, 'H', "host", "Wolframe host", QCommandLine::Optional },
		{ QCommandLine::Option, 'p', "port", "Wolframe port", QCommandLine::Optional },
		{ QCommandLine::Switch, 'S', "secure", "connect securely via SSL", QCommandLine::Optional },
		{ QCommandLine::Switch, 'l', "local", "Run with local data and form loader", QCommandLine::Optional },
		{ QCommandLine::Option, 'v', "verbose", "verbose level", QCommandLine::Optional },
		{ QCommandLine::Option, 'c', "client-cert-file", "client certificate to present to the server (default: ./certs/client.crt)", QCommandLine::Optional },
		{ QCommandLine::Option, 'k', "client-key-file", "client key file (default: ./private/client.key)", QCommandLine::Optional },
		{ QCommandLine::Option, 'C', "CA-cert-file", "certificate file containing the CA (default: ./certs/CAclient.cert.pem)", QCommandLine::Optional },
		QCOMMANDLINE_CONFIG_ENTRY_END
	};
	
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
		
	m_cmdline->parse( );
}

void MainWindow::switchFound( const QString &name )
{
	qDebug( ) << "switch" << name;
	if( name == "local" ) {
		m_loadMode = Local;
	} else if( name == "secure" ) {
		m_secure = true;
	}
}

void MainWindow::optionFound( const QString &name, const QVariant &value )
{
	qDebug( ) << "option" << name << "with" << value;
	if( name == "host" ) {
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
	m_wolframeClient = new WolframeClient( m_host, m_port, m_secure, m_clientCertFile, m_clientKeyFile, m_CACertFile );

// create debuging terminal
	m_debugTerminal = new DebugTerminal( m_wolframeClient, this );
	debugTerminal = m_debugTerminal;
	qInstallMsgHandler( &myMessageOutput );
	qDebug( ) << "Debug window initialized";

// connect the wolframe client to protocols, authenticate
	switch( m_loadMode ) {
		case Local:
			break;
		case Network:
			if( !m_wolframeClient->syncConnect( ) ) {
				qWarning( ) << "Can't connect to Wolframe daemon!";
				m_loadMode = Local;
				QCoreApplication::quit( );
			} else {
				connect( m_wolframeClient, SIGNAL( mechsReceived( QStringList ) ),
					this, SLOT( mechsReceived( QStringList ) ) );
				m_wolframeClient->auth( );
			}
			break;
		default:
			qWarning( ) << "Illegal load mode" << m_loadMode;
			QCoreApplication::quit( );
	}

	connect( m_wolframeClient, SIGNAL( error( QString ) ),
		this, SLOT( wolframeError( QString ) ) );

// a Qt UI loader for the main theme window
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled ( true );

// for testing, load lists of available forms from the files system,
// pass the form loader to the FormWidget
	switch( m_loadMode ) {
		case Local:
			m_formLoader = new FileFormLoader( "forms", "i18n" );
			m_dataLoader = new FileDataLoader( "data" );
			break;
		case Network:
			m_formLoader = new NetworkFormLoader( m_wolframeClient );
			m_dataLoader = new NetworkDataLoader( m_wolframeClient );
			break;
		default:
			qWarning( ) << "Illegal load mode" << m_loadMode;
			QCoreApplication::quit( );
	}

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

// set default language to the system language
	m_currentLanguage = QLocale::system( ).name( );

// load default theme
	loadTheme( QString( QLatin1String( "windows" ) ) );

// load language resources, repaints the whole interface if necessary
	loadLanguage( QLocale::system( ).name( ) );	
}

void MainWindow::wolframeError( QString error )
{
	qDebug( ) << error;
}

void MainWindow::mechsReceived( QStringList mechs )
{
	LoginDialog* loginDialog = new LoginDialog( m_wolframeClient, this );
	
	connect( loginDialog, SIGNAL( authenticationOk( ) ),
		this, SLOT( authenticationOk( ) ) );
		
	connect( loginDialog, SIGNAL( authenticationFailed( ) ),
		this, SLOT( authenticationFailed( ) ) );
		
	loginDialog->exec( );
	delete loginDialog;
}

void MainWindow::authenticationOk( )
{
	qDebug( ) << "authentication succeeded";
}

void MainWindow::authenticationFailed( )
{
	qDebug( ) << "authentication failed";
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
	QString themesFolder( QLatin1String( "themes/" ) + theme + QLatin1Char( '/' ) );

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
	qApp->setStyleSheet( QLatin1String( "file:///" ) + QFileInfo( qss ).absoluteFilePath( ) );
	
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
// construct a menu showing all languages
	QMenu *languageMenu = qFindChild<QMenu *>( m_ui, "menuLanguages" );
	languageMenu->clear( );
	QActionGroup *languageGroup = new QActionGroup( languageMenu );
	languageGroup->setExclusive( true );
	foreach( QString language, languages ) {
		QLocale locale( language );
		QAction *action = new QAction( locale.languageToString( locale.language( ) ) + " (" + language + ")", languageGroup );
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
	if( language == "en_US" ) {
		m_currentLanguage = language;
		return;
	}

// install new ones, first the ones of the theme, then the ones of the current form
// all other forms will reinstall the correct language when called again
	QTranslator *translator = new QTranslator( this );
	if( !translator->load( "qtclient." + language, "themes/" + m_currentTheme ) ) {
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
	qDebug( ) << "Form " << form << " selected";
	if( form != m_currentForm )
		loadForm( form );
}

void MainWindow::loadForm( QString name )
{
// delegate form loading to form widget
	m_formWidget->loadForm( name );

// remember the name of the current form
	m_currentForm = name;
}

void MainWindow::formLoaded( QString name )
{
// also set language of the form widget,
// but wait till the form got loaded, otherwise we get races!
	m_formWidget->loadLanguage( m_currentLanguage );
}

void MainWindow::on_actionExit_triggered( )
{
	disconnect( m_wolframeClient, SIGNAL( error( QString ) ), 0, 0 );
	m_debugTerminal = 0;
	debugTerminal = 0;
	
	close( );
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

} // namespace QtClient
} // namespace _Wolframe
