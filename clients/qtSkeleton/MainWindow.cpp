/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#include "MainWindow.hpp"
#include "FileDataLoader.hpp"
#include "NetworkDataLoader.hpp"
#include "settings.hpp"
#include "PreferencesDialog.hpp"
#include "global.hpp"
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
	m_uiLoader( 0 ),
	m_wolframeClient( 0 ), m_settings( ),
	m_languages( ), m_language( ),
	m_mdiArea( 0 ), m_subWinGroup( 0 ),
	m_terminating( false ), m_debugTerminal( 0 ), m_debugTerminalAction( 0 ),
	m_modalDialog( 0 )
{
// setup designer UI
	m_ui.setupUi( this );

// read parameters, first and only one is the optional configurartion files
// containint the settings
	parseArgs( );

// settings override built-in defaults
	readSettings( );

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
	if( m_uiLoader ) {
		delete m_uiLoader;
		m_uiLoader = 0;
	}
}

// --- command line argument handling

void MainWindow::parseArgs( )
{
	QStringList args = QApplication::arguments( );
	foreach( QString arg, args ) {
		qDebug( ) << arg;
	}
	if( args.size( ) > 1 ) {
		m_settings = args[1];
	}
}

// -- initialization

void MainWindow::initialize( )
{
// install custom output handler (mainly for Unix debugging)
	qInstallMsgHandler( &myMessageOutput );

// a Qt UI loader for the main theme window and also used by all form widgets
	m_uiLoader = new QUiLoader( );
	//m_uiLoader->setLanguageChangeEnabled( true );
	m_uiLoader->addPluginPath( "plugins" );
	m_uiLoader->addPluginPath( "." );
	QStringList paths = m_uiLoader->pluginPaths( );
	qDebug( ) << "Will load custom widget plugins from" << paths;

// ..same for the data loader
	switch( settings.dataLoadMode ) {
		case LoadMode::FILE:
//			m_dataLoader = new FileDataLoader( settings.dataLoaderDir );
			break;

		case LoadMode::NETWORK:
			// skip, delay, needs a working connection for this
			break;

		case LoadMode::UNDEFINED:
			break;
	}

//// link the form loader for form loader notifications needed by the main window
//// (list of forms for form menu, list of language for language picker)
//	connect( m_formLoader, SIGNAL( languageCodesLoaded( QStringList ) ),
//		this, SLOT( languageCodesLoaded( QStringList ) ) );
//	connect( m_formLoader, SIGNAL( formListLoaded( QStringList ) ),
//		this, SLOT( formListLoaded( QStringList ) ) );

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
		} else {
// missing a MDI area, so we disable the m_mdi flag
			settings.mdi = false;
		}
	}

// in local file UI and data mode we can load the form right away, we don't
// wait for the user to log in
	if( settings.uiLoadMode == LoadMode::FILE && settings.dataLoadMode == LoadMode::FILE ) {
		restoreStateAndPositions( );
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

//// now that we have a menu where we can add things, we start the form list loading
//	m_formLoader->initiateListLoad( );

// load language codes for language picker
	loadLanguages( );

// load language resources, repaints the whole interface if necessary
	loadLanguage( m_language );

// auto login for developers
	if( settings.autoLogin )
		on_actionLogin_triggered( );
}

//void MainWindow::CreateFormWidget( const QString &name )
//{
//	m_formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, settings.debug );

//	connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
//		this, SLOT( formLoaded( QString ) ) );
//	connect( m_formWidget, SIGNAL( formModal( QString ) ),
//		this, SLOT( formModal( QString ) ) );
//	connect( m_formWidget, SIGNAL( error( QString ) ),
//		this, SLOT( formError( QString ) ) );

//	setCentralWidget( m_formWidget );
//	m_formWidget->setLanguage( m_language );

//	loadForm( name );
//}

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

//void MainWindow::updateActionShortcuts( )
//{
//	foreach( QAction *action, findChildren<QAction *>( ) ) {
//		QString s = FormWidget::readDynamicStringProperty( action, "defaultShortcut" );
//		if( !s.isEmpty( ) ) {
//			QKeySequence::StandardKey shortcut = defaultKeySequenceFromString( s );
//			if( shortcut != QKeySequence::UnknownKey ) {
//				QKeySequence seq( shortcut );
//				if( !seq.isEmpty( ) ) {
//					action->setShortcuts( defaultKeySequenceFromString( s ) );
//					qDebug( ) << "ACTION" << action << "gets default shortcut" << s;
//				} else {
//					qDebug( ) << "ACTION" << action << "keeps shortcuts from ui resource" << action->shortcuts( );
//				}
//			}
//		}
//	}
//}

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

//	if( settings.uiLoadMode == LoadMode::NETWORK ) {
//		delete m_uiLoader;
//		m_uiLoader = 0;
//	}

//	if( settings.dataLoadMode == LoadMode::NETWORK ) {
//		delete m_dataLoader;
//		m_dataLoader = 0;
//	}

	updateMenusAndToolbars( );

	statusBar( )->showMessage( tr( "Terminated" ) );

	if( m_terminating ) {
		close( );
	}
}

void MainWindow::wolframeError( QString error )
{
	QMessageBox::warning( this, tr( "Server error" ), error, QMessageBox::Ok );

	updateMenusAndToolbars( );
}

void MainWindow::authOk( )
{
	qDebug( ) << "authentication succeeded";

	statusBar( )->showMessage( tr( "Ready" ) );

//// ...and data loaders
//	if( settings.dataLoadMode == LoadMode::NETWORK ) {
//		m_dataLoader = new NetworkDataLoader( m_wolframeClient, settings.debug );
//	}

	restoreStateAndPositions( );

// update status of menus and toolbars
	updateMenusAndToolbars( );
}

void MainWindow::authFailed( )
{
	qDebug( ) << "authentication failed";
}

//void MainWindow::loadLanguages( )
//{
//// get the list of available languages in the forms
//	m_formLoader->initiateGetLanguageCodes( );
//}

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
		if( language == m_language ) action->setChecked( true );
	}
	languageMenu->addActions( languageGroup->actions( ) );
	connect( languageGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( languageSelected( QAction * ) ) );
}

void MainWindow::languageSelected( QAction *action )
{
	QString language = action->data( ).toString( );
	if( language != m_language )
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

//void MainWindow::loadLanguage( QString language )
//{
//	qDebug( ) << "Switching interface language to " << language;

//// change language on global level
//	switchTranslator( m_translatorApp, QString( "qtclient.%1.qm" ).arg( language ), "i18n" );
//	switchTranslator( m_translatorQt, QString( "qt_%1.qm" ).arg( language ), "/usr/share/qt/translations/" );

//// also set language of the form widget(s)
//	if( settings.mdi ) {
//		foreach( QMdiSubWindow *w, m_mdiArea->subWindowList( ) ) {
//			FormWidget *f = qobject_cast<FormWidget *>( w->widget( ) );
//			f->setLanguage( language );
//			f->reload( );
//		}
//	} else {
//		if( m_formWidget ) {
//			m_formWidget->setLanguage( language );
//			m_formWidget->reload( );
//		}
//	}

//	m_language = language;
//}

void MainWindow::changeEvent( QEvent* _event )
{
	if( _event )	{
		if ( _event->type() == QEvent::LanguageChange )
			m_ui.retranslateUi( this );
		else if ( _event->type() == QEvent::LocaleChange )	{
			QString locale = QLocale::system( ).name( );
			locale.truncate( locale.lastIndexOf( '_' ) );
			loadLanguage( locale );
		}
	}

	QMainWindow::changeEvent( _event );
}

void MainWindow::formListLoaded( QStringList forms )
{
	m_forms = forms;
}

//void MainWindow::loadForm( QString name )
//{
//// delegate form loading to form widget
//	if( m_formWidget )
//		m_formWidget->loadForm( name );
//}

//void MainWindow::endModal( )
//{
//	qDebug( ) << "endModal";

//// restore wiring in main frame
//	connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
//		this, SLOT( formLoaded( QString ) ) );
//	connect( m_formWidget, SIGNAL( formModal( QString ) ),
//		this, SLOT( formModal( QString ) ) );
//	connect( m_formWidget, SIGNAL( error( QString ) ),
//		this, SLOT( formError( QString ) ) );
//	connect( m_formWidget,SIGNAL( destroyed( ) ),
//		this, SLOT( updateMenusAndToolbars( ) ) );

//	m_modalDialog->close( );
//	m_modalDialog->deleteLater( );

//	// hacky: should go without, especially because we loose data already
//	// entered in the parent dialog this way..
//	m_formWidget->reload( );
//}

//void MainWindow::formModal( QString name )
//{
//	m_modalDialog = new QDialog( this );

//	FormWidget *formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, m_modalDialog, settings.debug );

//	connect( formWidget, SIGNAL( formLoaded( QString ) ),
//		this, SLOT( formLoaded( QString ) ) );
//	connect( formWidget, SIGNAL( formModal( QString ) ),
//		this, SLOT( formModal( QString ) ) );
//	connect( formWidget, SIGNAL( error( QString ) ),
//		this, SLOT( formError( QString ) ) );
//	connect( formWidget,SIGNAL( destroyed( ) ),
//		this, SLOT( updateMenusAndToolbars( ) ) );
//	connect( formWidget,SIGNAL( closed( ) ),
//		this, SLOT( endModal( ) ) );

//// we are modal, so tempoarily we have to disconnect the parent form from
//// the main window in order not to trigger funny results
//	disconnect( m_formWidget, SIGNAL( formLoaded( QString ) ), 0, 0 );
//	disconnect( m_formWidget, SIGNAL( formModal( QString ) ), 0, 0 );
//	disconnect( m_formWidget, SIGNAL( error( QString ) ), 0, 0 );
//	disconnect( m_formWidget, SIGNAL( destroyed( ) ), 0, 0 );

//	QVBoxLayout *l = new QVBoxLayout( m_modalDialog );
//	l->addWidget( formWidget );

//	formWidget->setGlobals( m_formWidget->globals( ) );
//	formWidget->setLanguage( m_language );
//	formWidget->loadForm( name, true );

//	connect( m_modalDialog, SIGNAL( rejected( ) ),
//		this, SLOT( endModal( ) ) );

//	m_modalDialog->show( );
//}

//void MainWindow::formLoaded( QString /*name*/ )
//{
//// in MDI mode update the title of the sub window, otherwise update window title
//	if( settings.mdi ) {
//		QMdiSubWindow *mdiSubWindow = m_mdiArea->activeSubWindow( );
//		if( mdiSubWindow ) {
//			QString title = m_formWidget->windowTitle( );
//			mdiSubWindow->setWindowTitle( title );

//			QIcon icon = m_formWidget->getWindowIcon( );
//			if( !icon.isNull( ) ) {
//				qDebug( ) << "Setting window icon" << m_formWidget;
//				mdiSubWindow->setWindowIcon( icon );
//			} else {
//				qDebug( ) << "Setting application icon";
//				mdiSubWindow->setWindowIcon( windowIcon( ) );
//			}

//			m_mdiArea->update( );

//			QAction *action = m_revSubWinMap.value( mdiSubWindow );
//			if( action ) {
//				int idx = action->data( ).toInt( );
//				QString text = composeWindowListTitle( idx, title );
//				action->setText( text );
//			}
//		}
//	} else {
//		setWindowTitle( tr( "Wolframe Qt Client - %1" ).arg( m_formWidget->windowTitle( ) ) );
//	}
//}

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
	QMessageBox::critical( this, tr( "Form error" ), error, QMessageBox::Ok );
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

	if( settings.uiLoadMode == LoadMode::NETWORK || settings.dataLoadMode == LoadMode::NETWORK ) {
		if( m_wolframeClient ) {
			m_wolframeClient->disconnect( );
		} else {
			close( );
		}
	} else {
// terminate brutally in local mode (this is for a connection from the debug window)
		if( m_wolframeClient )
			disconnect( m_wolframeClient, SIGNAL( error( QString ) ), 0, 0 );

		if( settings.uiLoadMode == LoadMode::FILE && settings.dataLoadMode == LoadMode::FILE ) {
			storeStateAndPositions( );
		}

		close( );
	}
}

void MainWindow::restoreStateAndPositions( )
{
// restore main window position and size
	move( settings.mainWindowPos );
	resize( settings.mainWindowSize );

// load initial form, load forms and position of windows from settings,
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
// default it tiling MDI subwindows, no better option
			m_mdiArea->tileSubWindows( );
		}
	} else {
		if( settings.saveRestoreState && settings.states.size( ) > 0 ) {
			WinState state = settings.states[0];
			CreateFormWidget( state.form );
		} else {
			CreateFormWidget( "init" );
		}
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
//				FormWidget *f = qobject_cast<FormWidget *>( w->widget( ) );
//				state.form = f->form( );
				state.position = w->pos( );
				state.size = w->size( );
				settings.states.append( state );
			}
		} else {
			settings.states.clear( );
//			if( m_formWidget ) {
//				WinState state;
//				state.form = m_formWidget->form( );
//				state.position = m_formWidget->pos( );
//				state.size = m_formWidget->size( );
//				settings.states.append( state );
//			}
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

//void MainWindow::on_actionOpenForm_triggered( )
//{
//	FormChooseDialog d( m_forms, this );
//	if( d.exec( ) == QDialog::Accepted ) {
//		QString form = d.form( );
//		loadForm( form );
//	}
//}

//void MainWindow::on_actionReload_triggered( )
//{
//	m_formWidget->reload( );
//}

// -- MDI mode

//QMdiSubWindow *MainWindow::CreateMdiSubWindow( const QString &form )
//{
//	FormWidget *formWidget = new FormWidget( m_formLoader, m_dataLoader, m_uiLoader, this, settings.debug );

//	connect( formWidget, SIGNAL( formLoaded( QString ) ),
//		this, SLOT( formLoaded( QString ) ) );
//	connect( formWidget, SIGNAL( formModal( QString ) ),
//		this, SLOT( formModal( QString ) ) );
//	connect( formWidget, SIGNAL( error( QString ) ),
//		this, SLOT( formError( QString ) ) );
//	connect( formWidget,SIGNAL( destroyed( ) ),
//		this, SLOT( updateMenusAndToolbars( ) ) );

//	QMdiSubWindow *mdiSubWindow = m_mdiArea->addSubWindow( formWidget );
//	mdiSubWindow->setAttribute( Qt::WA_DeleteOnClose );

//	m_formWidget = formWidget; // ugly dirty hack, must ammend later
//	formWidget->show( );
//	formWidget->setLanguage( m_language );
//	loadForm( form );

//	mdiSubWindow->resize( mdiSubWindow->sizeHint( ) );

//	return mdiSubWindow;
//}

void MainWindow::subWindowSelected( QAction *action )
{
	QMdiSubWindow *w = m_subWinMap.value( action );
	m_mdiArea->setActiveSubWindow( w );
}

//void MainWindow::subWindowChanged( QMdiSubWindow *w )
//{
//	if( !w ) return;

//	m_formWidget = qobject_cast<FormWidget *>( w->widget( ) );

//	updateWindowMenu( );
//}

//void MainWindow::on_actionOpenFormNewWindow_triggered( )
//{
//	FormChooseDialog d( m_forms, this );
//	if( d.exec( ) == QDialog::Accepted ) {
//		(void)CreateMdiSubWindow( d.form( ) );
//	}

//	updateMenusAndToolbars( );
//}

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
		( settings.uiLoadMode == LoadMode::FILE && settings.dataLoadMode == LoadMode::FILE ) ||
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
//		m_statusBarConn->setToolTip( tr( "Status: online" ) );
		m_statusBarConn->setToolTip( tr( "Status: connected to server %1" ).arg( m_wolframeClient->serverName()) );
		m_statusBarConn->setEnabled( true );
	} else {
		m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/disconnected.png" ) );
		m_statusBarConn->setToolTip( tr( "Status: offline" ) );
		m_statusBarConn->setEnabled( false );
	}
	if( m_wolframeClient && m_wolframeClient->isEncrypted( ) ) {
		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/encrypted.png" ) );
		m_statusBarSSL->setToolTip( tr( "Encryption: %1" ).arg( m_wolframeClient->encryptionName()) );
		m_statusBarSSL->setEnabled( true );
	} else {
		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/unencrypted.png" ) );
		m_statusBarSSL->setToolTip( tr( "Encryption: N/A" ) );
		m_statusBarSSL->setEnabled( false );
	}

// logged in or logged out?
	activateAction( "actionOpenForm",
		( ( settings.uiLoadMode == LoadMode::FILE && settings.dataLoadMode == LoadMode::FILE )
		|| ( m_wolframeClient && m_wolframeClient->isConnected( ) ) )
		&& ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) );
	activateAction( "actionReload",
		( settings.uiLoadMode == LoadMode::FILE && settings.dataLoadMode == LoadMode::FILE ) ||
		( m_wolframeClient && ( !settings.mdi || ( settings.mdi && nofSubWindows( ) > 0 ) ) ) );

	if( settings.uiLoadMode == LoadMode::NETWORK || settings.dataLoadMode == LoadMode::NETWORK ) {
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

		m_selectedConnection = loginDlg->selectedConnection( );

// no SSL compiled in and the user picks a secure connection, warn him,
// don't blindly connect
#ifndef WITH_SSL
	if( m_selectedConnection.SSL ) {
		QMessageBox::critical( this, tr( "Parameters error"),
			"No SSL support is compiled in, can't open a secure connection" );
		delete loginDlg;
		return;
	}
#endif

// create a Wolframe protocol client
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
	} /*else {
		delete m_formWidget;
		m_formWidget = 0;
	}*/

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
