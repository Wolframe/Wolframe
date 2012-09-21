//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>
#include <QtUiTools>

#include "FormLoader.hpp"
#include "DebugTerminal.hpp"
#include "WolframeClient.hpp"
#include "FormWidget.hpp"

namespace _Wolframe {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
		virtual ~MainWindow( );
	
	private:
		QWidget *m_ui;			// main window from theme
		FormWidget *m_formWidget;	// current active form
		QString m_currentTheme;		// the name of the currently selected theme
		QString m_currentForm;		// the name of the form currently visible
		FormLoader *m_formLoader;	// form loader (visible form)
		DebugTerminal *m_debugTerminal;	// protocol debug terminal (interactive)
		WolframeClient *m_wolframeClient; // the client protocol class
		QString m_currentLanguage;	// code of the current interface language
		QUiLoader *m_uiLoader;		// the designer UI loader

		void initialize( );
		void populateThemesMenu( );
		void loadTheme( QString themeName );
		void loadLanguages( );
		void loadForm( QString formName );
		void loadLanguage( QString language );

	private Q_SLOTS:
		void themeSelected( QAction *action );
		void formSelected( QAction *action );
		void languageSelected( QAction *action );

		void formListLoaded( );

		void on_actionExit_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
		void on_actionDebugTerminal_triggered( bool checked );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _MAIN_WINDOW_HPP_INCLUDED
