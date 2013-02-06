//
// MainWindow.cpp
//

#include "MainWindow.hpp"
#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"
#include "NetworkFormLoader.hpp"
#include "NetworkDataLoader.hpp"
#include "settings.hpp"
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
	m_dataLoader( 0 ), m_wolframeClient( 0 ),
	m_loginDialog( 0 ), m_settings( ),
	m_languages( ), m_language( ), m_mdiArea( 0 ),
	m_subWinGroup( 0 ),
	m_terminating( false ), m_debugTerminal( 0 ), m_debugTerminalAction( 0 )
{
// setup designer UI
	m_ui.setupUi( this );
	
// read arguments for the '-s <setting file>' parameter
	parseArgs( );

// settings override built-in defaults
	readSettings( );

// command line options override settings
	parseArgs( );

	initialize( );
}

static bool _debug = false;
static DebugTerminal *_debugTerminal = 0;

void MainWindow::readSettings( )
{
	if( m_settings.isNull( ) ) {
// read from standard config location (.config/Wolframe or registry)
		settings.read( ORGANIZATION_NAME, APPLICATION_NAME );
	} else {
// read from configuration file given as '-s xxx.conf' on the command line
		settings.read( m_settings );
	}
	
	_debug = settings.debug;
	
	if( settings.locale == SYSTEM_LANGUAGE ) {
		m_language = QLocale::system( ).name( );
	} else {
		m_language = settings.locale;
	}
}

static void myMessageOutput( QtMsgType type, const char *msg )
{
	switch( type ) {
		case QtDebugMsg:
			if( _debug ) {
				if( _debugTerminal ) {
					_debugTerminal->sendComment( msg );
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
	if( settings.mdi ) {
		if( m_mdiArea )
			m_mdiArea->closeAllSubWindows( );
	} else {
		if( m_formWidget ) {
			delete m_formWidget;
			m_formWidget = 0;
		}
	}
	if( m_wolframeClient ) {
		delete m_wolframeClient;
		m_wolframeClient = 0;
	}
	if( m_debugTerminal ) {
		delete m_debugTerminal;
		m_debugTerminal = 0;
		_debugTerminal = 0;
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

// --- command line argument handling

void MainWindow::parseArgs( )
{
	const struct QCommandLineConfigEntry conf[] =
	{
		{ QCommandLine::Option, 'c', "configuration", "Use configuration from this file (default .config/Wolframe/Wolframe Client.conf)", QCommandLine::Optional },
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
}

void MainWindow::optionFound( const QString &name, const QVariant &value )
{
	qDebug( ) << "option" << name << "with" << value;
	if( name == "configuration" ) {
		m_settings = value.toString( );
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

// -- initialization

void MainWindow::initialize( )
{	
// install custom output handler (mainly for Unix debugging)
	qInstallMsgHandler( &myMessageOutput );

// a Qt UI loader for the main theme window and also used by all form widgets
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled( true );
	m_uiLoader->addPluginPath( "plugins" );
	m_uiLoader->addPluginPath( "." );
	QStringList paths = m_uiLoader->pluginPaths( );
	qDebug( ) << "Will load custom widget plugins from" << paths;

// for testing, load lists of available forms from the files system or
// a local sqlite database, pass the form loader to the FormWidget
	switch( settings.uiLoadMode ) {
		case LocalFile:
			m_formLoader = new FileFormLoader( settings.uiFormsDir, settings.uiFormTranslationsDir, settings.uiFormResourcesDir );
			break;

		case Network:
			// skip, delay
			break;
		
		case Unknown:
			break;
	}

// ..same for the data loader
	switch( settings.dataLoadMode ) {
		case LocalFile:
			m_dataLoader = new FileDataLoader( settings.dataLoaderDir );
			break;

		case Network:
			// skip, delay, needs a working connection for this
			break;

		case Unknown:
			break;
	}

// link the form loader for form loader notifications needed by the main window
// (list of forms for form menu, list of language for language picker)
	connect( m_formLoader, SIGNAL( languageCodesLoaded( QStringList ) ),
		this, SLOT( languageCodesLoaded( QStringList ) ) );
	connect( m_formLoader, SIGNAL( formListLoaded( QStringList ) ),
		this, SLOT( formListLoaded( QStringList ) ) );

// set default language to the system language
	m_currentLanguage = m_language;

// create central widget, either as MDI area or as one form widget
	if( settings.mdi ) {
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
		} else {
// missing a MDI area, so we disable the m_mdi flag
			settings.mdi = false;
		}
	}

// in local file UI and data mode we can load the form right away
	if( settings.uiLoadMode == LocalFile && settings.dataLoadMode == LocalFile ) {
		if( settings.mdi ) {
			(void)CreateMdiSubWindow( "init" );
		} else {
			CreateFormWidget( "init" );
		}
	}

// add the menu entries for the developer mode
	if( settings.developEnabled ) {
		addDeveloperMenu( );
	}

// update shortcuts to standard ones
	updateActionShortcuts( );
	
// add connection and encryption state indicators to status bar
	addStatusBarIndicators( );

// update menus and toolbars
	updateMenusAndToolbars( );
			
// now that we have a menu where we can add things, we start the form list loading
	m_formLoader->initiateListLoad( );

// load language codes for language picker
	loadLanguages( );

// load language resources, repaints the whole interface if necessary
	loadLanguage( m_currentLanguage );

// restore main window position and size
	move( settings.mainWindowPos );
	resize( settings.mainWindowSize );	

// restore subwindow position states
	if( settings.mdi ) {
		// TODO
	} else {
		// ignore position and size as they are fixed anyway
	}
}

void MainWindow::CreateFormWidget( const QString &name )
{
	m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, settings.debug );

	connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
		this, SLOT( formLoaded( QString ) ) );
	connect( m_formWidget, SIGNAL( error( QString ) ),
		this, SLOT( formError( QString ) ) );
		
	setCentralWidget( m_formWidget );
		
	loadForm( name );
}

void MainWindow::activateAction( const QString name, bool enabled )
{
	QAction *action = findChild<QAction *>( name );
	if( action ) {
		action->setEnabled( enabled );
	}
}

QKeySequence::StandardKey MainWindow::defaultKeySequenceFromString( const QString s )
{
	if( s == "QKeySequence::NextChild" ) {
		return QKeySequence::NextChild;
	} else if( s == "QKeySequence::PreviousChild" ) {
		return QKeySequence::PreviousChild;
	} else if( s == "QKeySequence::Print" ) {
		return QKeySequence::Print;
	} else if( s == "QKeySequence::Quit" ) {
		return QKeySequence::Quit;
	} else if( s == "QKeySequence::Undo" ) {
		return QKeySequence::Undo;
	} else if( s == "QKeySequence::Redo" ) {
		return QKeySequence::Redo;
	} else if( s == "QKeySequence::Cut" ) {
		return QKeySequence::Cut;
	} else if( s == "QKeySequence::Copy" ) {
		return QKeySequence::Copy;
	} else if( s == "QKeySequence::Paste" ) {
		return QKeySequence::Paste;
	} else if( s == "QKeySequence::Delete" ) {
		return QKeySequence::Delete;
	} else if( s == "QKeySequence::SelectAll" ) {
		return QKeySequence::SelectAll;
	} else if( s == "QKeySequence::Refresh" ) {
		return QKeySequence::Refresh;
	} else if( s == "QKeySequence::Open" ) {
		return QKeySequence::Open;
	} else if( s == "QKeySequence::New" ) {
		return QKeySequence::New;
	} else if( s == "QKeySequence::Close" ) {
		return QKeySequence::Close;
	} else if( s == "QKeySequence::Preferences" ) {
		return QKeySequence::Preferences;
	} else if( s == "QKeySequence::HelpContents" ) {
		return QKeySequence::HelpContents;
	} else {
		return QKeySequence::UnknownKey;
	}
}

void MainWindow::updateActionShortcuts( )
{
	foreach( QAction *action, findChildren<QAction *>( ) ) {
		QString s = FormWidget::readDynamicStringProperty( action, "defaultShortcut" );
		if( !s.isEmpty( ) ) {
			QKeySequence::StandardKey shortcut = defaultKeySequenceFromString( s );
			if( shortcut != QKeySequence::UnknownKey ) {
				QKeySequence seq( shortcut );
				if( !seq.isEmpty( ) ) {
					action->setShortcuts( defaultKeySequenceFromString( s ) );
					qDebug( ) << "ACTION" << action << "gets default shortcut" << s;
				} else {
					qDebug( ) << "ACTION" << action << "keeps shortcuts from ui resource" << action->shortcuts( );
				}
			}
		}
	}
}

void MainWindow::addStatusBarIndicators( )
{
	m_statusBarConn = new QLabel( this );
	m_statusBarConn->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusBar( )->addPermanentWidget( m_statusBarConn );
	m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/disconnected.png" ) );
	m_statusBarConn->setToolTip( tr( "Status: offline" ) );
	m_statusBarConn->setEnabled( false );

	m_statusBarSSL = new QLabel( this );
	m_statusBarSSL->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusBar( )->addPermanentWidget( m_statusBarSSL );
	m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/unencrypted.png" ) );
	m_statusBarSSL->setToolTip( tr( "Encryption: N/A" ) );
	m_statusBarSSL->setEnabled( false );
}

// --- handling protocol changes (connection states and errors)

void MainWindow::connected( )
{
	m_wolframeClient->auth( );		
}

void MainWindow::mechsReceived( QStringList /* mechs */ )
{
	m_wolframeClient->mech( "NONE" );
}

void MainWindow::disconnected( )
{
	m_wolframeClient->deleteLater( );
	m_wolframeClient = 0;

	if( m_debugTerminal ) {
		m_debugTerminalAction->setChecked( false );
		m_debugTerminal->deleteLater( );
		m_debugTerminal = 0;
		_debugTerminal = 0;
	}
	
	if( settings.uiLoadMode == Network ) {
		delete m_uiLoader;
		m_uiLoader = 0;
	}
	
	if( settings.dataLoadMode == Network ) {
		delete m_dataLoader;
		m_dataLoader = 0;
	}

	updateMenusAndToolbars( );

	statusBar( )->showMessage( tr( "Terminated" ) );
	
	if( m_terminating ) {
		close( );
	}
}

void MainWindow::wolframeError( QString error )
{
	statusBar( )->showMessage( error, 6000 );
	
	updateMenusAndToolbars( );
}

void MainWindow::authOk( )
{
	qDebug( ) << "authentication succeeded";

	statusBar( )->showMessage( tr( "Ready" ) );
	
// create network based form ...
	if( settings.uiLoadMode == Network ) {
		m_formLoader = new NetworkFormLoader( m_wolframeClient );
	}

// ...and data loaders
	if( settings.dataLoadMode == Network ) {
		m_dataLoader = new NetworkDataLoader( m_wolframeClient, settings.debug );
	}

// load initial form, TODO: load forms and position of windows from settings,
// of none there, load init form in a MDI subwindow or directly
	if( settings.mdi ) {
		if( settings.saveRestoreState ) {
			for( int i = 0; i < settings.states.size( ); i++ ) {
				WinState state = settings.states[i];
				QMdiSubWindow *w = CreateMdiSubWindow( state.form );
				w->move( state.position );
				w->resize( state.size );
			}
		} else {
			(void)CreateMdiSubWindow( "init" );
		}
	} else {
		if( settings.saveRestoreState && settings.states.size( ) > 0 ) {
			WinState state = settings.states[0];
			CreateFormWidget( state.form );
		} else {
			CreateFormWidget( "init" );
		}
	}	

// update status of menus and toolbars
 	updateMenusAndToolbars( );
}

void MainWindow::authFailed( )
{
	qDebug( ) << "authentication failed";
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
        
// also set language of the form widget(s)
	if( settings.mdi ) {
		foreach( QMdiSubWindow *w, m_mdiArea->subWindowList( ) ) {
			FormWidget *f = qobject_cast<FormWidget *>( w->widget( ) );
			f->loadLanguage( language );
		}
	} else {
		if( m_formWidget ) {
			m_formWidget->loadLanguage( language );
		}
	}

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
	if( settings.mdi ) {
		QMdiSubWindow *mdiSubWindow = m_mdiArea->activeSubWindow( );
		if( mdiSubWindow ) {
			QString title = m_formWidget->windowTitle( );
			mdiSubWindow->setWindowTitle( title );

			QIcon icon = m_formWidget->getWindowIcon( );
			if( !icon.isNull( ) ) {
				qDebug( ) << "Setting window icon" << m_formWidget;
				mdiSubWindow->setWindowIcon( icon );
			} else {
				qDebug( ) << "Setting application icon";
				mdiSubWindow->setWindowIcon( windowIcon( ) );
			}
			
			m_mdiArea->update( );
			
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
	close( );
	QApplication::instance( )->exit( RESTART_CODE );
}

void MainWindow::on_actionExit_triggered( )
{
	m_terminating = true;

	if( settings.uiLoadMode == Network || settings.dataLoadMode == Network ) {
		if( m_wolframeClient ) {
			m_wolframeClient->disconnect( );
		} else {
			close( );
		}
	} else {
// terminate brutally in local mode
		if( m_wolframeClient )
			disconnect( m_wolframeClient, SIGNAL( error( QString ) ), 0, 0 );
		close( );
	}
}

void MainWindow::storeStateAndPositions( )
{
// save our own size and position
	if( settings.saveRestoreState ) {
		settings.mainWindowPos = pos( );
		settings.mainWindowSize = size( );
	}

// save position/size and state of subwindows (if wished)	
	if( settings.saveRestoreState ) {
		settings.states.clear( );
		if( settings.mdi ) {
			foreach( QMdiSubWindow *w, m_mdiArea->subWindowList( ) ) {
				WinState state;
				FormWidget *f = qobject_cast<FormWidget *>( w->widget( ) );
				state.form = f->form( );
				state.position = w->pos( );
				state.size = w->size( );
				settings.states.append( state );
			}
		} else {
			settings.states.clear( );
			if( m_formWidget ) {
				WinState state;
				state.form = m_formWidget->form( );
				state.position = m_formWidget->pos( );
				state.size = m_formWidget->size( );
				settings.states.append( state );
			}
		}
	}
}

void MainWindow::storeSettings( )
{
	if( m_settings.isEmpty( ) ) {
		settings.write( ORGANIZATION_NAME, APPLICATION_NAME );
	} else {
		settings.write( m_settings );
	}
}

void MainWindow::closeEvent( QCloseEvent *e )
{
	storeSettings( );
	
	e->accept( );
}

void MainWindow::on_actionPreferences_triggered( )
{
	PreferencesDialog prefs( settings, m_languages, this );
	if( prefs.exec( ) == QDialog::Accepted ) {
		qDebug( ) << "Reloading application";
		storeSettings( );
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

// -- form handling

void MainWindow::on_actionOpenForm_triggered( )
{
	FormChooseDialog d( m_forms, this );
	if( d.exec( ) == QDialog::Accepted ) {
		QString form = d.form( );
		loadForm( form );
	}
}

void MainWindow::on_actionReload_triggered( )
{
	m_formWidget->reload( );
}

// -- MDI mode

QMdiSubWindow *MainWindow::CreateMdiSubWindow( const QString &form )
{
	FormWidget *formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, settings.debug );

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
	
	return mdiSubWindow;
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

void MainWindow::on_actionOpenFormNewWindow_triggered( )
{
	FormChooseDialog d( m_forms, this );
	if( d.exec( ) == QDialog::Accepted ) {
		(void)CreateMdiSubWindow( d.form( ) );
	}

	updateMenusAndToolbars( );
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

void MainWindow::updateMdiMenusAndToolbars( )
{
// present new form menu entry if logged in
	activateAction( "actionOpenFormNewWindow",
		( settings.uiLoadMode == LocalFile && settings.dataLoadMode == LocalFile ) ||
		m_wolframeClient );

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

void MainWindow::updateMenusAndToolbars( )
{
// connection status
	if( m_wolframeClient && m_wolframeClient->isConnected( ) ) {
		m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/connected.png" ) );
		m_statusBarConn->setToolTip( tr( "Status: online" ) );
		m_statusBarConn->setEnabled( true );
	} else {
		m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/disconnected.png" ) );
		m_statusBarConn->setToolTip( tr( "Status: offline" ) );
		m_statusBarConn->setEnabled( false );
	}
	if( m_wolframeClient && m_wolframeClient->isEncrypted( ) ) {
		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/encrypted.png" ) );
		m_statusBarSSL->setToolTip( tr( "Encrypted" ) );
		m_statusBarSSL->setEnabled( true );
	} else {
		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/unencrypted.png" ) );
		m_statusBarSSL->setToolTip( tr( "Encryption: N/A" ) );
		m_statusBarSSL->setEnabled( false );	
	}
	
// logged in or logged out?
	activateAction( "actionOpenForm", 
		( ( settings.uiLoadMode == LocalFile && settings.dataLoadMode == LocalFile )
		|| m_wolframeClient )
		&& ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionReload",	
		( settings.uiLoadMode == LocalFile && settings.dataLoadMode == LocalFile ) ||
		( m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) ) );
		
	if( settings.uiLoadMode == Network || settings.dataLoadMode == Network ) {
		activateAction( "actionLogin", !m_wolframeClient || !m_wolframeClient->isConnected( ) );
		activateAction( "actionLogout", m_wolframeClient && m_wolframeClient->isConnected( ) );
	}
	
// MDI menus and toolbars
	if( settings.mdi ) {
		updateMdiMenusAndToolbars( );
	}

// edit menu (TODO: this is provisoric)
	activateAction( "actionUndo", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionRedo", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionCut", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionCopy", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionPaste", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionDelete", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionSelectAll", m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );	

// developer menu: debug terminal
	if( m_debugTerminalAction )
		m_debugTerminalAction->setEnabled( m_debugTerminal );
}

// -- logins/logouts/connections

void MainWindow::on_actionLogin_triggered( )
{
	QString	username;
//	QString	password;
	QString	connName;

	if ( settings.saveUsername )	{
		username = settings.lastUsername;
		connName = settings.lastConnection;
	}

	LoginDialog* loginDlg = new LoginDialog( username, connName,
						 settings.connectionParams );
	if( loginDlg->exec( ) == QDialog::Accepted ) {
// optionally remember old login data		
		if( settings.saveUsername ) {
			settings.lastUsername = loginDlg->username( );
			settings.lastConnection = loginDlg->selectedConnection( ).name;
		}

// create a Wolframe protocol client
		m_selectedConnection = loginDlg->selectedConnection( );
		m_wolframeClient = new WolframeClient( m_selectedConnection );

// create a debug terminal and attach it to the protocol client
	if( settings.debug && settings.developEnabled ) {
		m_debugTerminal = new DebugTerminal( m_wolframeClient, this );
		m_debugTerminal->setAttribute( Qt::WA_DeleteOnClose );
		_debugTerminal = m_debugTerminal;
		connect( m_wolframeClient, SIGNAL( lineSent( QString ) ),
			m_debugTerminal, SLOT( sendLine( QString ) ) );
		connect( m_debugTerminal,SIGNAL( destroyed( ) ),
			this, SLOT( removeDebugToggle( ) ) );
		qDebug( ) << "Debug window initialized";
	}

// catch signals from the network layer
		connect( m_wolframeClient, SIGNAL( error( QString ) ),
			this, SLOT( wolframeError( QString ) ) );
                connect( m_wolframeClient, SIGNAL( connected( ) ),
                        this, SLOT( connected( ) ) );
                connect( m_wolframeClient, SIGNAL( disconnected( ) ),
                        this, SLOT( disconnected( ) ) );
		connect( m_wolframeClient, SIGNAL( mechsReceived( QStringList ) ),
			this, SLOT( mechsReceived( QStringList ) ) );
		connect( m_wolframeClient, SIGNAL( authOk( ) ),
			this, SLOT( authOk( ) ) );
		connect( m_wolframeClient, SIGNAL( authFailed( ) ),
			this, SLOT( authFailed( ) ) );

// initiate connect
                m_wolframeClient->connect( );
	}
	
	delete loginDlg;
}

void MainWindow::on_actionLogout_triggered( )
{
	storeStateAndPositions( );
	storeSettings( );

	if( settings.mdi ) {
		m_mdiArea->closeAllSubWindows( );
	} else {
		delete m_formWidget;
		m_formWidget = 0;
	}
		
	m_wolframeClient->disconnect( );
}

void MainWindow::on_actionManageServers_triggered( )
{
	ManageServersDialog* serversDlg = new ManageServersDialog( settings.connectionParams );
	serversDlg->exec( );

	delete serversDlg;
}

// -- developer stuff

void MainWindow::showDebugTerminal( bool checked )
{
	if( m_debugTerminal ) {
		if( checked ) {
			m_debugTerminal->bringToFront( );
		} else {
			m_debugTerminal->hide( );
		}
	}
}

void MainWindow::removeDebugToggle( )
{
	m_debugTerminalAction->setChecked( false );
	_debugTerminal = 0;
}

void MainWindow::addDeveloperMenu( )
{
	QMenu *developerMenu = menuBar( )->addMenu( tr( "&Developer" ) );

	m_debugTerminalAction = new QAction( QIcon( ":/images/debug.png" ), tr( "&Debugging Terminal..." ), this );
	m_debugTerminalAction->setStatusTip( tr( "Open debug terminal showing the Wolframe protocol" ));
	m_debugTerminalAction->setCheckable( true );
	m_debugTerminalAction->setShortcut( QKeySequence( "Ctrl+Alt+D" ) );
	developerMenu->addAction( m_debugTerminalAction );
	
	QToolBar *developerToolBar = addToolBar( tr( "Developer" ));
	developerToolBar->addAction( m_debugTerminalAction );

	connect( m_debugTerminalAction, SIGNAL( toggled( bool ) ), this,
		SLOT( showDebugTerminal( bool ) ) );
}
