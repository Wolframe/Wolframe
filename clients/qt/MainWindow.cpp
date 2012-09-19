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

MainWindow::MainWindow( QWidget *_parent ) : QWidget( _parent ), m_ui( 0 ), m_form( 0 )
{
	// for testing, load form descriptions and data
	// from the local filesystem
	m_formLoader = new FileFormLoader( "forms", "i18n" );
	m_dataLoader = new FileDataLoader( "data" );
	m_dataHandler = new DataHandler( );
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled ( true );
	initialize( );
}

MainWindow::~MainWindow( )
{
	delete m_debugTerminal;
	delete m_wolframeClient;
	delete m_dataHandler;
	delete m_formLoader;
	delete m_dataLoader;
	delete m_uiLoader;
}

void MainWindow::initialize( )
{
// link the form loader for form loader notifications
	QObject::connect( m_formLoader, SIGNAL( formListLoaded( ) ),
		this, SLOT( formListLoaded( ) ) );
	QObject::connect( m_formLoader, SIGNAL( formLoaded( QString, QByteArray, QByteArray ) ),
		this, SLOT( formLoaded( QString, QByteArray, QByteArray ) ) );	

// link the data loader to our window
	QObject::connect( m_dataLoader, SIGNAL( dataLoaded( QString, QByteArray ) ),
		this, SLOT( dataLoaded( QString, QByteArray ) ) );
	QObject::connect( m_dataLoader, SIGNAL( dataSaved( QString ) ),
		this, SLOT( dataSaved( QString ) ) );

// create a Wolframe protocol client
	m_wolframeClient = new WolframeClient( );

// create debuging terminal
	m_debugTerminal = new DebugTerminal( m_wolframeClient, this );

// set default language to the system language
	m_currentLanguage = QLocale::system( ).name( );
		
// load default theme
	loadTheme( QString( QLatin1String( "windows" ) ) );
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

// wire standard actions in the menu by name (on_<object>_<signal>)
// (true autowiring doesn't work: from now signals to ghost signals everything
// happens)
	QAction *actionExit = qFindChild<QAction *>( m_ui, "actionExit" );
	QObject::connect( actionExit, SIGNAL( triggered( ) ), this, SLOT( on_actionExit_triggered( ) ) );

	QAction *actionAbout = qFindChild<QAction *>( m_ui, "actionAbout" );
	QObject::connect( actionAbout, SIGNAL( triggered( ) ), this, SLOT( on_actionAbout_triggered( ) ) );

	QAction *actionAboutQt = qFindChild<QAction *>( m_ui, "actionAboutQt" );
	QObject::connect( actionAboutQt, SIGNAL( triggered( ) ), this, SLOT( on_actionAboutQt_triggered( ) ) );

	QAction *actionDebugTerminal = qFindChild<QAction *>( m_ui, "actionDebugTerminal" );
	QObject::connect( actionDebugTerminal, SIGNAL( triggered( bool ) ), this, SLOT( on_actionDebugTerminal_triggered( bool ) ) ); 
	// TODO: for this to work the SAME object must implement
	// the slots!
	// QMetaObject::connectSlotsByName( this );
	
// copy over the location of the old window to the new one
// also copy over the current form, don't destroy the old ui,
// events could be outstanding (deleteLater marks the widget 
// for deletion, will be deleted when returning into the event
// loop)
	if( oldUi ) {
		m_ui->move( oldUi->pos( ) );
		oldUi->hide( );
		oldUi->deleteLater( );
	}

// show the new gui
	m_ui->show( );
	
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
	if( m_form ) loadForm( m_currentForm );
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
	qDebug( ) << "Switching interface language to " << language;
	if( language == m_currentLanguage ) {
		return;
	}

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
	if( !translator->load( "MainWindow." + language, "themes/" + m_currentTheme ) ) {
		qDebug( ) << "Error while loading translations for theme " <<
			m_currentTheme << " for locale " << m_currentLanguage;
	}
	QCoreApplication::instance( )->installTranslator( translator );

	m_currentLanguage = language;
}

void MainWindow::changeEvent( QEvent *e )
{
	QWidget::changeEvent( e );
	qDebug( ) << "changeEvent";
	
	switch( e->type( ) ) {
		case QEvent::LanguageChange:
// QLoader ui retranslate automatically
			qDebug( ) << "retranslate!";
			break;
			
		default:
			break;
	}
}

void MainWindow::formSelected( QAction *action )
{		
	QString form = action->text( );
	qDebug( ) << "Form " << form << " selected";
	if( form != m_currentForm )
		loadForm( form );
}

void MainWindow::loadForm( QString form )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	qDebug( ) << "Initiating form load for " << form;
	m_formLoader->initiateFormLoad( form, QLocale( m_currentLanguage ) );
}

void MainWindow::formLoaded( QString name, QByteArray form, QByteArray localization )
{
	qDebug( ) << "Form " << name << " loaded";

// get list of all translators for this form and delete them
	const QList<QTranslator *> oldTranslators( m_form->findChildren<QTranslator *>( ) );
	foreach( QTranslator *translator, oldTranslators ) {
		QCoreApplication::instance( )->removeTranslator( translator );
	}
	qDeleteAll( oldTranslators );
	
// read the form and construct it
	QWidget *oldForm = m_form;
	QBuffer buf( &form );
	m_form = m_uiLoader->load( &buf, m_ui );
	buf.close( );

// install translation files for this form
	QTranslator *translator = new QTranslator( m_form );
	if( !translator->load( (const uchar *)localization.constData( ), localization.length( ) ) ) {
		qDebug( ) << "Error while loading translations for form " <<
			name << " for locale " << m_currentLanguage;
	}
	QCoreApplication::instance( )->installTranslator( translator );

// add it to the main window, disable old form
	QVBoxLayout *l = qFindChild<QVBoxLayout *>( m_ui, "mainAreaLayout" );
	l->addWidget( m_form );

	if( oldForm ) {
		m_form->move( oldForm->pos( ) );
		oldForm->hide( );
		l->removeWidget( oldForm );
		oldForm->deleteLater( );
	}
	m_form->show( );

// remember the name of the current form
	m_currentForm = name;

// initiate load of form data
	qDebug( ) << "Initiating loading of form data for form " << name;
	
	m_dataLoader->initiateDataLoad( name );

// connect standard form actions
	QMetaObject::connectSlotsByName( this );
	
// not busy anymore
	qApp->restoreOverrideCursor();
}

void MainWindow::on_buttons_accepted( )
{
	qDebug( ) << "Form accepted";
	
	QByteArray xml;
	m_dataHandler->writeFormData( m_currentForm, m_form, &xml );
	
	m_dataLoader->initiateDataSave( m_currentForm, xml );
}

void MainWindow::on_buttons_rejected( )
{
	qDebug( ) << "Form rejected";
	m_dataLoader->initiateDataLoad( m_currentForm );
}

void MainWindow::dataSaved( QString name )
{	
	qDebug( ) << "Saved data for form " << name;
}

void MainWindow::dataLoaded( QString name, QByteArray xml )
{
	qDebug( ) << "Loaded data for form " << name << ":\n"
		<< xml;
	
	m_dataHandler->readFormData( name, m_form, xml );
}

void MainWindow::themeSelected( QAction *action )
{
	QString theme = action->text( );
	if( theme != m_currentTheme )
		loadTheme( theme );
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
