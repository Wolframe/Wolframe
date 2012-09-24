//
// MainWindow.cpp
//

#include "MainWindow.hpp"
#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"

#include <QtGui>
#include <QBuffer>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QtAlgorithms>

#include <QDebug>

namespace _Wolframe {
	namespace QtClient {

MainWindow::MainWindow( QWidget *_parent ) : QWidget( _parent ), m_ui( 0 )
{
// a Qt UI loader for the main theme window
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled ( true );

// for testing, load lists of available forms from the files system,
// pass the form loader to the FormWidget
	m_formLoader = new FileFormLoader( "forms", "i18n" );	

	m_formWidget = new FormWidget( m_formLoader, this );
	
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

void MainWindow::initialize( )
{
// link the form loader for form loader notifications
	QObject::connect( m_formLoader, SIGNAL( formListLoaded( ) ),
		this, SLOT( formListLoaded( ) ) );

// get notified if the form widget changes a form
	QObject::connect( m_formWidget, SIGNAL( formLoaded( QString ) ),
		this, SLOT( formLoaded( QString ) ) );

// create a Wolframe protocol client
	m_wolframeClient = new WolframeClient( );

// create debuging terminal
	m_debugTerminal = new DebugTerminal( m_wolframeClient, this );

// set default language to the system language
	m_currentLanguage = QLocale::system( ).name( );

// load default theme
	loadTheme( QString( QLatin1String( "windows" ) ) );

// load language resources, repaints the whole interface if necessary
	loadLanguage( QLocale::system( ).name( ) );		
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
	QObject::connect( themesGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( themeSelected( QAction * ) ) );
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
	QStringList languages = m_formLoader->getLanguageCodes( );

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
	QObject::connect( languageGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( languageSelected( QAction * ) ) );
}

void MainWindow::formListLoaded( )
{
// get the list of available forms
	QStringList forms = m_formLoader->getFormNames( );

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
	QObject::connect( formGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( formSelected( QAction * ) ) );
	
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
