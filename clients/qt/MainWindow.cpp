//
// MainWindow.cpp
//

#include "MainWindow.hpp"

#include <QtUiTools>
#include <QtGui>

namespace _SMERP {
	namespace QtClient {

MainWindow::MainWindow( QWidget *_parent ) : QWidget( _parent )
{
	initialize( );
}

void MainWindow::initialize( )
{
// load default theme
	loadTheme( QString( QLatin1String( "windows" ) ) );

// load all themes possible to pick
	populateThemesMenu( );
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

	themes_menu->addActions( themesGroup->actions( ) );
	QObject::connect( themesGroup, SIGNAL( triggered( QAction * ) ), this, SLOT( themeSelected( QAction * ) ) );
}

void MainWindow::loadTheme( QString theme )
{
#if 0
	current_theme = theme;

	QWidget *old_ui = ui;
	QUiLoader loader;

	// go to theme directory
	QString theme_folder( QLatin1String( "themes/" ) + theme + QLatin1Char( '/' ) );
	loader.setWorkingDirectory( theme_folder );

	// load the main window
	QFile file( theme_folder + QLatin1String( "Test1.ui" ) );
	file.open( QFile::ReadOnly );
	ui = loader.load( &file, this );
	file.close( );

	// set stylesheet of the application (has impact on the whole application)
	QFile qss( theme_folder + QLatin1String( "Test1.qss" ) );
	qApp->setStyleSheet( QLatin1String( "file:///" ) + QFileInfo( qss ).absoluteFilePath( ) );

	// initialize menu actions and connects signals to slots
	initialize( );

	// copy over the location of the old window to the new one
	// also copy over the current form, don't destroy the old ui,
	// events could be outstanding (deleteLater marks the widget
	// for deletion, will be deleted when returning into the event
	// loop)
	if( old_ui ) {
		ui->move( old_ui->pos( ) );
		old_ui->hide( );
		old_ui->deleteLater( );
	}

	// set preferred base style
	QStyle *base_style = QStyleFactory::create( ui->property( "themeBaseStyle" ).toString( ) );
	if( base_style ) {
		base_style->setParent( ui );
		ui->setStyle( base_style );
	}

	// show new UI
	ui->show( );

#endif

	qApp->setOverrideCursor( Qt::BusyCursor );

// set working directory to theme
	QString themesFolder( QLatin1String( "themes/" ) + theme + QLatin1Char( '/' ) );

// tell the loader that this is the working directory
	QUiLoader loader;
	loader.setWorkingDirectory( themesFolder );

// load the main window (which is empty) and provides basic functions like
// theme switching, login, exit, about, etc. (to start unauthenticated)
	QFile file( themesFolder + QLatin1String( "MainWindow.ui" ) );
	file.open( QFile::ReadOnly );
	m_ui = loader.load( &file, this );
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

// remember current theme
	m_currentTheme = theme;

	qApp->restoreOverrideCursor( );
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

} // namespace QtClient
} // namespace _SMERP
