//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>

#include "FileFormLoader.hpp"

namespace _SMERP {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
		~MainWindow( );
	
	private:
		QWidget *m_ui;			// main window from theme
		QString m_currentTheme;		// the name of the currently selected theme
		FileFormLoader *m_formLoader;	// a simple form loader for testing (for now)

		void initialize( );
		void populateThemesMenu( );
		void loadTheme( QString themeName );

	private Q_SLOTS:
		void themeSelected( QAction *action );
		void on_actionExit_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _MAIN_WINDOW_HPP_INCLUDED
