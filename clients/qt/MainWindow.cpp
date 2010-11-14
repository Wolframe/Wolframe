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

// wire standard actions by name (on_<object>_<signal>)
	QMetaObject::connectSlotsByName( this );

// remember current theme
	m_currentTheme = theme;
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
