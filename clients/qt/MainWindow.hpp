//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>

#include "FileFormLoader.hpp"
#include "DebugTerminal.hpp"

namespace _SMERP {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
		virtual ~MainWindow( );
	
	private:
		QWidget *m_ui;			// main window from theme
		QWidget *m_form;		// current active form
		QString m_currentTheme;		// the name of the currently selected theme
		QString m_currentForm;		// the name of the form currently visible
		FileFormLoader *m_formLoader;	// a simple form loader for testing (for now)
		DebugTerminal *m_debugTerminal;	// protocol debug terminal (interactive)

		void initialize( );
		void populateThemesMenu( );
		void loadTheme( QString themeName );
		void loadForm( QString formName );

	private Q_SLOTS:
		void themeSelected( QAction *action );
		void formSelected( QAction *action );
		void formListLoaded( );
		void formLoaded( QString name, QByteArray xml );

		void on_actionExit_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
		void on_actionDebugTerminal_triggered( bool checked );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _MAIN_WINDOW_HPP_INCLUDED
