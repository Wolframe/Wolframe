//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>

namespace _SMERP {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
	
	private:
		QWidget *m_ui;		// main window from theme

		void initialize( );

	private Q_SLOTS:
		void on_actionExit_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _MAIN_WINDOW_HPP_INCLUDED
